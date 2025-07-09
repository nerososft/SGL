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
    uint outputWidthStride;// 输出图像Y平面的行跨度（字节）
    uint outputHeightStride;// 输出图像高度跨度（通常为实际高度）
    uint format;// 可保持为1表示NV12格式
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

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.inputWidth, filterParams.inputHeight)))) {
        return;
    }

    // 计算输入图像中的像素索引
    uint inputIndex = coord.y * filterParams.inputWidth + coord.x;

    // 从输入缓冲区读取像素并解包为RGBA
    vec4 rgba = unpackColor(inputImage.pixels[inputIndex]);

    // 提取RGB分量（忽略Alpha）
    vec3 rgb = rgba.rgb;

    // RGBA到YUV转换 (使用BT.601标准)
    float y =  0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b;
    float u = -0.14713 * rgb.r - 0.28886 * rgb.g + 0.436 * rgb.b + 0.5;
    float v =  0.615 * rgb.r - 0.51499 * rgb.g - 0.10001 * rgb.b + 0.5;

    // 计算Y平面的输出索引（NV12格式的前半部分是Y平面）
    uint yIndex = coord.y * filterParams.outputWidthStride + coord.x;

    // 计算UV平面的输出索引（NV12格式的后半部分是UV交错平面）
    // UV平面是Y平面尺寸的一半（在每个维度上）
    uvec2 uvCoord = uvec2(coord.x / 2, coord.y / 2);
    uint uvIndex = filterParams.inputWidth * filterParams.inputHeight +
    uvCoord.y * filterParams.outputWidthStride +
    uvCoord.x * 2;// 每个UV对占两个字节

    // 将Y值写入输出缓冲区（转换为8位无符号整数）
    outputImage.pixels[yIndex] = uint(y * 255.0);

    // 仅在处理偶数坐标时写入UV值（因为UV平面是Y平面的1/4大小）
    if ((coord.x % 2 == 0) && (coord.y % 2 == 0)) {
        // 将U值写入UV索引位置
        outputImage.pixels[uvIndex] = uint(u * 255.0);
        // 将V值写入UV索引+1位置
        outputImage.pixels[uvIndex + 1] = uint(v * 255.0);
    }
}