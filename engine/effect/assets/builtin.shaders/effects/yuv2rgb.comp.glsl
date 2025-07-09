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
    uint inputWidthStride;
    uint inputHeightStride;
    uint format;// 0: I420
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

// BT.601
const mat3 yuvToRgb = mat3(
1.0, 0.0, 1.13983,
1.0, -0.39465, -0.58060,
1.0, 2.03211, 0.0
);

const vec3 yuvOffset = vec3(-16.0/255.0, -128.0/255.0, -128.0/255.0);

// 从缓冲区读取一个字节
uint readByte(uint byteOffset) {
    uint wordOffset = byteOffset / 4;
    uint byteInWord = byteOffset % 4;

    uint word = inputImage.pixels[wordOffset];
    return (word >> (byteInWord * 8)) & 0xFF;
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    // 使用正确的输出尺寸参数
    if (any(greaterThanEqual(coord, uvec2(filterParams.outputWidth, filterParams.outputHeight)))) {
        return;
    }

    // 输出像素的索引
    uint outputIndex = coord.y * filterParams.outputBytesPerLine + coord.x;
    float y, u, v;

    // 输入图像的实际尺寸（假设输入和输出尺寸相同，除非使用了stride）
    uint inputWidth = filterParams.inputWidthStride;
    uint inputHeight = filterParams.inputHeightStride;

    // 如果stride未设置，则使用输出尺寸
    if (inputWidth == 0) inputWidth = filterParams.outputWidth;
    if (inputHeight == 0) inputHeight = filterParams.outputHeight;

    if (filterParams.format == 0) { // I420 (YUV 4:2:0)
        // 计算Y分量的字节偏移量
        uint yOffset = coord.y * inputWidth + coord.x;
        y = float(readByte(yOffset)) / 255.0;

        // 计算UV坐标（下采样）
        uvec2 uvCoord = uvec2(coord.x / 2, coord.y / 2);
        uint uvWidth = inputWidth / 2;

        // 计算U分量的字节偏移量
        uint uOffset = inputWidth * inputHeight + uvCoord.y * uvWidth + uvCoord.x;
        u = float(readByte(uOffset)) / 255.0;

        // 计算V分量的字节偏移量
        uint vOffset = inputWidth * inputHeight + (inputWidth * inputHeight / 4) + uvCoord.y * uvWidth + uvCoord.x;
        v = float(readByte(vOffset)) / 255.0;
    }
    else if (filterParams.format == 1) { // NV12 (YUV 4:2:0 with interleaved UV)
        // 计算Y分量的字节偏移量
        uint yOffset = coord.y * inputWidth + coord.x;
        y = float(readByte(yOffset)) / 255.0;

        // 计算UV坐标（下采样）
        uvec2 uvCoord = uvec2(coord.x / 2, coord.y / 2);
        uint uvWidth = inputWidth / 2;

        // 计算UV分量的字节偏移量（NV12中U和V是交替存储的）
        uint uvOffset = inputWidth * inputHeight + (uvCoord.y * uvWidth + uvCoord.x) * 2;

        // 读取U和V分量
        u = float(readByte(uvOffset)) / 255.0;
        v = float(readByte(uvOffset + 1)) / 255.0;
    }
    else if (filterParams.format == 2) { // I444 (YUV 4:4:4)
        // 计算Y分量的字节偏移量
        uint yOffset = coord.y * inputWidth + coord.x;
        y = float(readByte(yOffset)) / 255.0;

        // 计算U分量的字节偏移量
        uint uOffset = inputWidth * inputHeight + coord.y * inputWidth + coord.x;
        u = float(readByte(uOffset)) / 255.0;

        // 计算V分量的字节偏移量
        uint vOffset = inputWidth * inputHeight * 2 + coord.y * inputWidth + coord.x;
        v = float(readByte(vOffset)) / 255.0;
    }
    else {
        // 默认情况，全部设为0
        y = 0.0;
        u = 0.0;
        v = 0.0;
    }

    vec3 yuv = vec3(y, u, v) + yuvOffset;
    vec3 rgb = yuvToRgb * yuv;

    outputImage.pixels[outputIndex] = packColor(vec4(1.0, rgb));
}