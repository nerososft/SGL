#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) readonly buffer InputImageStorageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) writeonly buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform FilterParams {
    uint width;
    uint height;
    uint channels;
    uint bytesPerLine;
    uint targetWidth;
    uint targetHeight;
    uint type;
} filterParams;

// 预计算常数
const float INV_255 = 1.0 / 255.0;
const float PI = 3.14159265359;
const float HALF_PI = 1.57079632679;

// ABGR - 使用更高效的位操作
uint packColor(vec4 color) {
    uvec4 i = uvec4(clamp(color * 255.0 + 0.5, 0.0, 255.0));
    return (i.a << 24) | (i.b << 16) | (i.g << 8) | i.r;
}

// ABGR - 使用整数运算避免浮点除法
vec4 unpackColor(uint color) {
    return vec4(
        float(color & 0xFF) * INV_255,
        float((color >> 8) & 0xFF) * INV_255,
        float((color >> 16) & 0xFF) * INV_255,
        float((color >> 24) & 0xFF) * INV_255
    );
}

// 三次卷积权重计算 (Keys' Cubic Interpolation)
float cubicWeight(float d) {
    d = abs(d);
    // 使用多项式计算，避免条件分支
    float d2 = d * d;
    float d3 = d2 * d;

    return (d < 1.0) ? (4.0 + d3 * 3.0 - d2 * 6.0) / 6.0 :
           (d < 2.0) ? (8.0 - 12.0 * d + 6.0 * d2 - d3) / 6.0 : 0.0;
}

// Mitchell-Netravali 双三次插值权重函数
float cubicWeightNew(float x) {
    x = abs(x);
    float x2 = x * x;
    float x3 = x2 * x;
    float k = -0.75;

    return (x < 1.0) ? ((k + 2.0) * x3 - (k + 3.0) * x2 + 1.0) :
           (x < 2.0) ? (k * x3 - 5.0 * k * x2 + 8.0 * k * x - 4.0 * k) : 0.0;
}

// Lanczos插值权重函数 (a=2.0)
float lanczosWeight(float x) {
    x = abs(x);
    if (x >= 2.0) return 0.0;

    // 特殊情况处理
    if (x < 1e-5) return 1.0;

    float pi_x = PI * x;
    float pi_x_over_2 = HALF_PI * x;

    return sin(pi_x) * sin(pi_x_over_2) / (pi_x * pi_x_over_2);
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= filterParams.targetWidth || coord.y >= filterParams.targetHeight)
        return;

    // 预计算常用值
    uint stride = filterParams.bytesPerLine >> 2; // 除以4的等价操作
    vec2 targetSize = vec2(filterParams.targetWidth, filterParams.targetHeight);
    vec2 srcSize = vec2(filterParams.width, filterParams.height);

    // 计算源坐标
    vec2 uv = (vec2(coord) + 0.5) / targetSize;
    vec2 srcCoord = uv * srcSize;

    vec2 fractPart = fract(srcCoord);
    ivec2 base = ivec2(srcCoord - fractPart);

    vec4 finalColor = vec4(0);
    float totalWeight = 0.0;

    // 根据插值类型选择权重函数，避免循环内分支
    if (filterParams.type == 1) {
        // 使用立方卷积
        for (int y = -1; y <= 2; ++y) {
            for (int x = -1; x <= 2; ++x) {
                ivec2 samplePos = base + ivec2(x, y);
                vec2 d = vec2(x, y) - fractPart + 1.0;

                float wx = cubicWeight(d.x);
                float wy = cubicWeight(d.y);
                float weight = wx * wy;

                samplePos = clamp(samplePos, ivec2(0), ivec2(filterParams.width - 1, filterParams.height - 1));
                uint pixelIndex = samplePos.y * stride + samplePos.x;
                finalColor += unpackColor(inputImage.pixels[pixelIndex]) * weight;
                totalWeight += weight;
            }
        }
    }
    else if (filterParams.type == 2) {
        // 使用Mitchell-Netravali
        for (int y = -1; y <= 2; ++y) {
            for (int x = -1; x <= 2; ++x) {
                ivec2 samplePos = base + ivec2(x, y);
                vec2 d = vec2(x, y) - fractPart + 1.0;

                float wx = cubicWeightNew(d.x);
                float wy = cubicWeightNew(d.y);
                float weight = wx * wy;

                samplePos = clamp(samplePos, ivec2(0), ivec2(filterParams.width - 1, filterParams.height - 1));
                uint pixelIndex = samplePos.y * stride + samplePos.x;
                finalColor += unpackColor(inputImage.pixels[pixelIndex]) * weight;
                totalWeight += weight;
            }
        }
    }
    else if (filterParams.type == 3) {
        // 使用Lanczos
        for (int y = -1; y <= 2; ++y) {
             for (int x = -1; x <= 2; ++x) {
                ivec2 samplePos = base + ivec2(x, y);
                vec2 d = vec2(x, y) - fractPart + 1.0;

                float wx = lanczosWeight(d.x);
                float wy = lanczosWeight(d.y);
                float weight = wx * wy;

                samplePos = clamp(samplePos, ivec2(0), ivec2(filterParams.width - 1, filterParams.height - 1));
                uint pixelIndex = samplePos.y * stride + samplePos.x;
                finalColor += unpackColor(inputImage.pixels[pixelIndex]) * weight;
                totalWeight += weight;
            }
        }
    }

    // 使用混合避免除零分支
    finalColor /= max(totalWeight, 1e-6);
    uint outputIndex = coord.y * filterParams.targetWidth + coord.x;
    outputImage.pixels[outputIndex] = packColor(clamp(finalColor, 0.0, 1.0));
}