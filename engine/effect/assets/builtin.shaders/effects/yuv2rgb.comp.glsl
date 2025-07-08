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
    if (any(greaterThanEqual(coord, uvec2(filterParams.outputWidth, filterParams.outputHeight)))) {
        return;
    }

    // 计算当前像素的字节偏移量
    uint byteOffset = coord.y * filterParams.outputBytesPerLine + coord.x;

    float y, u, v;

    // Y平面大小是 width * height
    uint ySize = filterParams.outputWidth * filterParams.outputHeight;
    // U和V平面大小是 width*height/4
    uint uvSize = ySize / 4;

    // 读取Y分量
    uint yIndex = coord.y * filterParams.inputWidthStride + coord.x;
    y = float(inputImage.pixels[yIndex] & 0xFF) / 255.0;

    // 计算UV分量的坐标(UV是Y的1/2分辨率)
    uint uvX = coord.x / 2;
    uint uvY = coord.y / 2;

    // 读取U分量
    uint uIndex = ySize + uvY * (filterParams.inputWidthStride / 2) + uvX;
    u = float(inputImage.pixels[uIndex] & 0xFF) / 255.0;

    // 读取V分量
    uint vIndex = ySize + uvSize + uvY * (filterParams.inputWidthStride / 2) + uvX;
    v = float(inputImage.pixels[vIndex] & 0xFF) / 255.0;

    vec3 yuv = vec3(y, u, v) + yuvOffset;
    vec3 rgb = yuvToRgb * yuv;

    uint index = coord.y * filterParams.outputWidth + coord.x;
    outputImage.pixels[index] = packColor(vec4(rgb, 1.0));
}