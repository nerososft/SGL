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
    uint format; // 0: I420, 1: NV12, etc.
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

// ABGR
vec4 unpackColor(uint color) {
    return vec4(
    float((color) & 0xFF) / 255.0f,
    float((color >> 8) & 0xFF) / 255.0f,
    float((color >> 16) & 0xFF) / 255.0f,
    float((color >> 24) & 0xFF) / 255.0f
    );
}

// RGB转YUV的系数
const mat3 rgbToYuv = mat3(
0.299,     0.587,     0.114,
-0.14713, -0.28886,  0.436,
0.615,    -0.51499, -0.10001
);

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) {
        return;
    }

    // 计算输入像素索引
    uint pixelIndex = coord.y * filterParams.width + coord.x;
    // 解包ABGR颜色
    vec4 abgrColor = unpackColor(inputImage.pixels[pixelIndex]);
    // 转换为RGB
    vec3 rgbColor = abgrColor.rgb;

    // 应用RGB到YUV的转换
    vec3 yuvColor = rgbToYuv * rgbColor;

    // 计算YUV420输出的索引
    uint yPlaneSize = filterParams.width * filterParams.height;
    uint uvWidth = filterParams.width / 2;
    uint uvHeight = filterParams.height / 2;

    // 写入Y分量（亮度）
    outputImage.pixels[pixelIndex] = uint(clamp(yuvColor.x, 0.0, 1.0) * 255.0);

    // 对UV分量进行2x2下采样
    if (coord.x % 2 == 0 && coord.y % 2 == 0) {
        uint uvX = coord.x / 2;
        uint uvY = coord.y / 2;
        uint uIndex = yPlaneSize + uvY * uvWidth + uvX;
        uint vIndex = yPlaneSize + uvWidth * uvHeight + uvY * uvWidth + uvX;

        // 写入U（Cb）和V（Cr）分量
        outputImage.pixels[uIndex] = uint(clamp(yuvColor.y + 0.5, 0.0, 1.0) * 255.0);
        outputImage.pixels[vIndex] = uint(clamp(yuvColor.z + 0.5, 0.0, 1.0) * 255.0);
    }
}