#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageStorageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform FilterParams {
    uint width;
    uint height;
    uint channels;
    uint bytesPerLine;
    uint format;// 0: I420, 1: NV12, etc.
} filterParams;

// ABGR
uint packColor(vec4 color) {
    return (
    (uint(clamp(color.a, 0.0, 1.0) * 255.0) << 24) |
    (uint(clamp(color.b, 0.0, 1.0) * 255.0) << 16) |
    (uint(clamp(color.g, 0.0, 1.0) * 255.0) << 8) |
    uint(clamp(color.r, 0.0, 1.0) * 255.0)
    );
}

// YUV 到 RGB 的转换矩阵 (BT.601 标准)
const mat3 yuvToRgb = mat3(
1.0, 0.0, 1.13983,
1.0, -0.39465, -0.58060,
1.0, 2.03211, 0.0
);

// 偏移量
const vec3 yuvOffset = vec3(-16.0/255.0, -128.0/255.0, -128.0/255.0);

// 从 uint 缓冲区读取单字节
uint readByte(uint bufferIndex, uint byteOffset) {
    uint word = inputImage.pixels[bufferIndex];
    return (word >> (byteOffset * 8)) & 0xFFu;
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) {
        return;
    }

    // 计算 Y 分量的字节索引
    uint yByteIndex = coord.y * filterParams.bytesPerLine + coord.x;
    uint yWordIndex = yByteIndex / 4u;// 每个 uint 包含 4 个字节
    uint yByteOffset = yByteIndex % 4u;

    // 读取 Y 值 (8 位无符号整数)
    float y = float(readByte(yWordIndex, yByteOffset)) / 255.0;

    // 计算 U 和 V 分量的字节索引
    uvec2 uvCoord = coord / 2u;
    uint uvWidth = filterParams.bytesPerLine / 2;
    uint uvHeight = filterParams.height / 2;

    // U 平面紧跟在 Y 平面后面
    uint uByteIndex = filterParams.width * filterParams.height +
    uvCoord.y * uvWidth + uvCoord.x;
    uint uWordIndex = uByteIndex / 4u;
    uint uByteOffset = uByteIndex % 4u;

    // V 平面在 U 平面后面
    uint vByteIndex = filterParams.width * filterParams.height +
    uvWidth * uvHeight +
    uvCoord.y * uvWidth + uvCoord.x;
    uint vWordIndex = vByteIndex / 4u;
    uint vByteOffset = vByteIndex % 4u;

    // 读取 U 和 V 值
    float u = float(readByte(uWordIndex, uByteOffset)) / 255.0;
    float v = float(readByte(vWordIndex, vByteOffset)) / 255.0;

    // 应用偏移量并转换到 RGB
    vec3 yuv = vec3(y, u, v) + yuvOffset;
    vec3 rgb = yuvToRgb * yuv;

    // 写入输出图像 (ABGR 格式)
    outputImage.pixels[yByteIndex / 4u] = packColor(vec4(rgb, 1.0));
}