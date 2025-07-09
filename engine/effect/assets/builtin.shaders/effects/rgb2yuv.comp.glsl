#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageStorageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform FilterParams {
    uint inputWidth;
    uint inputHeight;
    uint inputChannels;
    uint inputBytesPerLine;
    uint outputWidthStride; // 输出图像Y平面的行跨度（uint数）
    uint outputHeightStride; // 输出图像高度跨度（通常为实际高度）
    uint format; // 可保持为1表示NV12格式
} filterParams;

// ABGR
vec4 unpackColor(uint color) {
    return vec4(
    float((color) & 0xFF) / 255.0f,
    float((color >> 8) & 0xFF) / 255.0f,
    float((color >> 16) & 0xFF) / 255.0f,
    float((color >> 24) & 0xFF) / 255.0f
    );
}

// 将4个8位值打包到一个uint中
uint pack4Bytes(uint b0, uint b1, uint b2, uint b3) {
    return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.inputWidth, filterParams.inputHeight)))) {
        return;
    }

    // 从输入缓冲区读取像素并解包为RGBA
    uint inputIndex = coord.y * filterParams.inputWidth + coord.x;
    vec4 rgba = unpackColor(inputImage.pixels[inputIndex]);

    // 提取RGB分量并转换为YUV
    vec3 rgb = rgba.rgb;
    float y =  0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b;
    float u = -0.14713 * rgb.r - 0.28886 * rgb.g + 0.436 * rgb.b + 0.5;
    float v =  0.615 * rgb.r - 0.51499 * rgb.g - 0.10001 * rgb.b + 0.5;

    // 确保YUV值在有效范围内
    y = clamp(y, 0.0, 1.0);
    u = clamp(u, 0.0, 1.0);
    v = clamp(v, 0.0, 1.0);

    // 将YUV值转换为8位整数
    uint yByte = min(uint(y * 255.0), 255u);
    uint uByte = min(uint(u * 255.0), 255u);
    uint vByte = min(uint(v * 255.0), 255u);

    // 计算Y平面的输出索引（考虑对齐）
    uint yLineOffset = coord.y * filterParams.outputWidthStride;  // 行偏移（uint数）
    uint yPixelOffset = coord.x / 4;                               // 像素在uint中的偏移
    uint yUintIndex = yLineOffset + yPixelOffset;                 // uint索引
    uint yByteOffset = coord.x % 4;                                // 字节偏移（0-3）

    // 计算UV平面的输出索引（考虑对齐）
    uvec2 uvCoord = uvec2(coord.x / 2, coord.y / 2);
    uint uvLineOffset = uvCoord.y * filterParams.outputWidthStride;  // UV行偏移
    uint uvPixelOffset = uvCoord.x / 2;                               // UV像素在uint中的偏移
    uint uvUintIndex = (filterParams.outputWidthStride * filterParams.outputHeightStride) +
    uvLineOffset + uvPixelOffset;                   // UV的uint索引
    uint uvByteOffset = uvCoord.x % 2;                                // UV字节偏移（0-1）

    // 写入Y值
    uint yMask = ~(0xFFu << (yByteOffset * 8));
    uint yValue = yByte << (yByteOffset * 8);
    outputImage.pixels[yUintIndex] = (outputImage.pixels[yUintIndex] & yMask) | yValue;

    // 仅在处理偶数坐标时写入UV值
    if ((coord.x % 2 == 0) && (coord.y % 2 == 0)) {
        uint uvMask = ~(0xFFu << (uvByteOffset * 8));
        uint uvValue = (uvByteOffset == 0 ? uByte : vByte) << (uvByteOffset * 8);
        outputImage.pixels[uvUintIndex] = (outputImage.pixels[uvUintIndex] & uvMask) | uvValue;
    }
}