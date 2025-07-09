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

// 打包RGBA颜色为ABGR格式的32位无符号整数
uint packColor(vec4 color) {
    return (
    (uint(clamp(color.a, 0.0, 1.0) * 255.0) << 24) |
    (uint(clamp(color.b, 0.0, 1.0) * 255.0) << 16) |
    (uint(clamp(color.g, 0.0, 1.0) * 255.0) << 8) |
    uint(clamp(color.r, 0.0, 1.0) * 255.0)
    );
}

// BT.601标准YUV到RGB转换矩阵
const mat3 yuvToRgb = mat3(
1.164, 0.0, 1.793,
1.164, -0.213, -0.533,
1.164, 2.112, 0.0
);

// YUV分量偏移校正（将[0,1]范围映射到标准YUV范围）
const vec3 yuvOffset = vec3(-16.0/255.0, -128.0/255.0, -128.0/255.0);

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

    float y, u, v;

    // 计算Y分量的字节偏移（Y平面每行跨度为inputWidthStride）
    uint yByteOffset = coord.y * filterParams.inputWidthStride + coord.x;
    y = float(readByte(yByteOffset)) / 255.0;// 转换为[0,1]范围

    // 计算UV分量的坐标（NV12的UV是4:2:0下采样，分辨率为Y的1/2）
    uint uvX = coord.x / 2;
    uint uvY = coord.y / 2;

    // NV12的UV平面在Y平面之后，每个UV对占用2字节（U和V各1字节）
    uint uvPlaneOffset = filterParams.inputHeightStride * filterParams.inputWidthStride;// Y平面总大小
    uint uvByteOffset = uvPlaneOffset + (uvY * filterParams.inputWidthStride) + (uvX * 2);// UV平面内偏移

    // 读取U和V分量（NV12中U在偶数偏移，V在奇数偏移）
    u = float(readByte(uvByteOffset)) / 255.0;// U分量
    v = float(readByte(uvByteOffset + 1)) / 255.0;// V分量

    // YUV到RGB转换
    vec3 yuv = vec3(y, u, v) + yuvOffset;// 应用偏移校正
    vec3 rgb = yuvToRgb * yuv;// 矩阵转换

    // 计算输出像素索引并写入结果（ABGR格式）
    uint outputIndex = coord.y * filterParams.outputWidth + coord.x;
    outputImage.pixels[outputIndex] = packColor(vec4(rgb, 1.0));
}