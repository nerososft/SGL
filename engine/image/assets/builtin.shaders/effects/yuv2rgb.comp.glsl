#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageStorageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform FilterParams {
    uint outputWidth;
    uint outputHeight;
    uint outputChannels;
    uint outputBytesPerLine;
    uint inputWidthStride;// 输入图像Y平面的行跨度（字节）
    uint inputHeightStride;// 输入图像高度跨度（通常为实际高度）
    uint format;// 可保持为1表示NV12格式
} filterParams;

// 从缓冲区指定字节偏移处读取一个字节
uint readByte(uint byteOffset) {
    uint wordOffset = byteOffset / 4;// 计算32位字的索引
    uint byteInWord = byteOffset % 4;// 计算字内字节偏移（0-3）

    uint word = inputImage.pixels[wordOffset];
    return (word >> (byteInWord * 8)) & 0xFF;// 提取目标字节
}

void main() {
    // 获取当前线程处理的像素坐标
    uvec2 coord = gl_GlobalInvocationID.xy;

    // 边界检查：如果坐标超出输出图像范围则退出
    if (any(greaterThanEqual(coord, uvec2(filterParams.outputWidth, filterParams.outputHeight)))) {
        return;
    }

    uint y, u, v;

    // 计算Y分量的字节偏移（Y平面每行跨度为inputWidthStride）
    uint yByteOffset = coord.y * filterParams.inputWidthStride + coord.x;
    y = readByte(yByteOffset);

    // 计算UV分量的坐标（NV12的UV是4:2:0下采样，分辨率为Y的1/2）
    uint uvX = coord.x / 2;
    uint uvY = coord.y / 2;

    // NV12的UV平面在Y平面之后，每个UV对占用2字节（U和V各1字节）
    uint uvPlaneOffset = filterParams.inputHeightStride * filterParams.inputWidthStride;// Y平面总大小
    uint uvByteOffset = uvPlaneOffset + (uvY * filterParams.inputWidthStride) + (uvX * 2);// UV平面内偏移

    // 读取U和V分量（NV12中U在偶数偏移，V在奇数偏移）
    u = readByte(uvByteOffset);
    v = readByte(uvByteOffset + 1);

    uint c = (y-16) * 298;
    uint d = u-128;
    uint e = v-128;

    uint r = (c + 409 * e + 128) / 256;
    uint g = (c - 100 * d - 208 * e + 128) / 256;
    uint b = (c + 516 * d + 128) / 256;

    uint outputIndex = coord.y * filterParams.outputWidth + coord.x;

    uint color = (
    (0xFF << 24) |
    (clamp(b, 0, 0xFF) << 16) |
    (clamp(g, 0, 0xFF) << 8) |
    clamp(r, 0, 0xFF));
    outputImage.pixels[outputIndex] = color;
}