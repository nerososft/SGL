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
} params;

// 预计算常数
const float INV_255 = 1.0 / 255.0;
const float PI = 3.14159265359;
const float HALF_PI = 1.57079632679;
const vec4 ZERO = vec4(0.0);
const vec4 ONE = vec4(1.0);
const uint MASK_8BIT = 0xFF;

float unpack8BitSingleChannel(uint packedValue, uint byteOffset) {
    uint eightBitValue = (packedValue >> (byteOffset * 8)) & MASK_8BIT;
    return float(eightBitValue) * INV_255;
}

uint pack8BitSingleChannel(uint originalPacked, float normalizedValue, uint byteOffset) {
    uint eightBitValue = uint(clamp(normalizedValue * 255.0 + 0.5, 0.0, 255.0));
    originalPacked &= ~(MASK_8BIT << (byteOffset * 8));
    originalPacked |= (eightBitValue << (byteOffset * 8));
    return originalPacked;
}

// 三次卷积权重计算 (Keys' Cubic Interpolation)
float cubicWeight(float d) {
    d = abs(d);
    float d2 = d * d;
    float d3 = d2 * d;

    float w = (4.0 + d3 * 3.0 - d2 * 6.0) * 0.166666667;
    w = mix(w, 0.0, step(1.0, d));

    float w2 = (8.0 - 12.0 * d + 6.0 * d2 - d3) * 0.166666667;
    return mix(w, w2, step(1.0, d) * (1.0 - step(2.0, d)));
}

// Mitchell-Netravali 双三次插值权重函数
float cubicWeightNew(float x) {
    x = abs(x);
    float x2 = x * x;
    float x3 = x2 * x;
    float k = -0.75;

    float w1 = (k + 2.0) * x3 - (k + 3.0) * x2 + 1.0;
    w1 = mix(w1, 0.0, step(1.0, x));

    float w2 = k * x3 - 5.0 * k * x2 + 8.0 * k * x - 4.0 * k;
    return mix(w1, w2, step(1.0, x) * (1.0 - step(2.0, x)));
}

// Lanczos插值权重函数 (a=2.0)
float lanczosWeight(float x) {
    x = abs(x);
    if (x >= 2.0) return 0.0;
    if (x < 1e-5) return 1.0;

    float pi_x = PI * x;
    float pi_x_over_2 = HALF_PI * x;

    return sin(pi_x) * sin(pi_x_over_2) / (pi_x * pi_x_over_2);
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= params.targetWidth || coord.y >= params.targetHeight) {
        return;
    }

    uint stride = params.bytesPerLine >> 2;
    vec2 targetSize = vec2(params.targetWidth, params.targetHeight);
    vec2 srcSize = vec2(params.width, params.height);
    vec2 uv = (vec2(coord) + 0.5) / targetSize;
    vec2 srcCoord = uv * srcSize;
    vec2 fractPart = fract(srcCoord);
    ivec2 base = ivec2(srcCoord - fractPart);
    vec2 d_base = -fractPart + 1.0;
    ivec2 sizeLimit = ivec2(params.width - 1, params.height - 1);
    float finalColor = 0.0;
    float totalWeight = 0.0;
    if (params.type == 1) { // 立方卷积
        for (int y = -1; y <= 2; ++y) {
            float wy = cubicWeight(float(y) + d_base.y);
            for (int x = -1; x <= 2; ++x) {
                float wx = cubicWeight(float(x) + d_base.x);
                float weight = wx * wy;
                if (weight < 1e-6) continue;
                ivec2 samplePos = clamp(base + ivec2(x, y), ivec2(0), sizeLimit);
                uint sampleGlobalIndex = uint(samplePos.x) + uint(samplePos.y) * params.width;
                uint uintIndex = sampleGlobalIndex / 4;
                uint byteOffset = sampleGlobalIndex % 4;
                uint packedValue = inputImage.pixels[uintIndex];
                float sampledColor = unpack8BitSingleChannel(packedValue, byteOffset);
                finalColor += sampledColor * weight;
                totalWeight += weight;
            }
        }
    } else if (params.type == 2) { // Mitchell-Netravali
        for (int y = -1; y <= 2; ++y) {
            float wy = cubicWeightNew(float(y) + d_base.y);
            for (int x = -1; x <= 2; ++x) {
                float wx = cubicWeightNew(float(x) + d_base.x);
                float weight = wx * wy;
                if (weight < 1e-6) continue;
                ivec2 samplePos = clamp(base + ivec2(x, y), ivec2(0), sizeLimit);
                uint sampleGlobalIndex = uint(samplePos.x) + uint(samplePos.y) * params.width;
                uint uintIndex = sampleGlobalIndex / 4;
                uint byteOffset = sampleGlobalIndex % 4;
                uint packedValue = inputImage.pixels[uintIndex];
                float sampledColor = unpack8BitSingleChannel(packedValue, byteOffset);
                finalColor += sampledColor * weight;
                totalWeight += weight;
            }
        }
    } else if (params.type == 3) { // Lanczos
        for (int y = -1; y <= 2; ++y) {
            float wy = lanczosWeight(float(y) + d_base.y);
            for (int x = -1; x <= 2; ++x) {
                float wx = lanczosWeight(float(x) + d_base.x);
                float weight = wx * wy;
                if (weight < 1e-6) continue;
                ivec2 samplePos = clamp(base + ivec2(x, y), ivec2(0), sizeLimit);
                uint sampleGlobalIndex = uint(samplePos.x) + uint(samplePos.y) * params.width;
                uint uintIndex = sampleGlobalIndex / 4;
                uint byteOffset = sampleGlobalIndex % 4;
                uint packedValue = inputImage.pixels[uintIndex];
                float sampledColor = unpack8BitSingleChannel(packedValue, byteOffset);
                finalColor += sampledColor * weight;
                totalWeight += weight;
            }
        }
    }

    finalColor = clamp(finalColor / max(totalWeight, 1e-6), 0.0, 1.0);
    uint targetGlobalIndex = coord.x + coord.y * params.targetWidth;
    uint targetUintIndex = targetGlobalIndex / 4;
    uint targetByteOffset = targetGlobalIndex % 4;
    uint targetPacked = pack8BitSingleChannel(0u, finalColor, targetByteOffset);
    outputImage.pixels[targetUintIndex] = targetPacked;
}