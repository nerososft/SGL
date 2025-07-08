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

// BT.601
const mat3 yuvToRgb = mat3(
1.0, 0.0, 1.13983,
1.0, -0.39465, -0.58060,
1.0, 2.03211, 0.0
);

const vec3 yuvOffset = vec3(-16.0/255.0, -128.0/255.0, -128.0/255.0);

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) {
        return;
    }

    // 计算当前像素的字节偏移量
    uint byteOffset = coord.y * filterParams.bytesPerLine + coord.x;

    // 计算该字节所在的uint索引和在uint内的字节位置
    uint uintIndex = byteOffset / 4;
    uint bytePos = byteOffset % 4;

    // 从uint中提取对应的字节，并归一化到[0,1]
    uint yByte;
    if (bytePos == 0) yByte = (inputImage.pixels[uintIndex] >> 0) & 0xFF;
    else if (bytePos == 1) yByte = (inputImage.pixels[uintIndex] >> 8) & 0xFF;
    else if (bytePos == 2) yByte = (inputImage.pixels[uintIndex] >> 16) & 0xFF;
    else yByte = (inputImage.pixels[uintIndex] >> 24) & 0xFF;
    float y = float(yByte) / 255.0;

    // 计算U分量的字节偏移量（U平面从width*height开始）
    uint uByteOffset = filterParams.width * filterParams.height + (coord.y/2) * (filterParams.width/2) + (coord.x/2);
    uint uUintIndex = uByteOffset / 4;
    uint uBytePos = uByteOffset % 4;

    // 从uint中提取U字节，并归一化到[0,1]
    uint uByte;
    if (uBytePos == 0) uByte = (inputImage.pixels[uUintIndex] >> 0) & 0xFF;
    else if (uBytePos == 1) uByte = (inputImage.pixels[uUintIndex] >> 8) & 0xFF;
    else if (uBytePos == 2) uByte = (inputImage.pixels[uUintIndex] >> 16) & 0xFF;
    else uByte = (inputImage.pixels[uUintIndex] >> 24) & 0xFF;
    float u = float(uByte) / 255.0;

    // 计算V分量的字节偏移量（V平面从width*height*5/4开始）
    uint vByteOffset = filterParams.width * filterParams.height * 5/4 + (coord.y/2) * (filterParams.width/2) + (coord.x/2);
    uint vUintIndex = vByteOffset / 4;
    uint vBytePos = vByteOffset % 4;

    // 从uint中提取V字节，并归一化到[0,1]
    uint vByte;
    if (vBytePos == 0) vByte = (inputImage.pixels[vUintIndex] >> 0) & 0xFF;
    else if (vBytePos == 1) vByte = (inputImage.pixels[vUintIndex] >> 8) & 0xFF;
    else if (vBytePos == 2) vByte = (inputImage.pixels[vUintIndex] >> 16) & 0xFF;
    else vByte = (inputImage.pixels[vUintIndex] >> 24) & 0xFF;
    float v = float(vByte) / 255.0;

    vec3 yuv = vec3(y, u, v) + yuvOffset;
    vec3 rgb = yuvToRgb * yuv;

    uint index = coord.y * filterParams.bytesPerLine + coord.x;
    outputImage.pixels[index] = packColor(vec4(rgb, 1.0));
}