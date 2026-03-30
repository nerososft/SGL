#version 450
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageStorageBuffer0 { uint pixels[]; } inputImage0;
layout (std430, binding = 1) buffer InputImageStorageBuffer1 { uint pixels[]; } inputImage1;
layout (std430, binding = 2) buffer InputImageStorageBuffer2 { uint pixels[]; } inputImage2;
layout (std430, binding = 3) buffer InputImageStorageBuffer3 { uint pixels[]; } inputImage3;
layout (std430, binding = 4) buffer InputImageStorageBuffer4 { uint pixels[]; } inputImage4;
layout (std430, binding = 5) buffer InputImageStorageBuffer5 { uint pixels[]; } inputImage5;
layout (std430, binding = 6) buffer InputImageStorageBuffer6 { uint pixels[]; } inputImage6;
layout (std430, binding = 7) buffer InputImageStorageBuffer7 { uint pixels[]; } inputImage7;
layout (std430, binding = 8) buffer InputImageStorageBuffer8 { uint pixels[]; } inputImage8;
layout (std430, binding = 9) buffer OutputImageStorageBuffer { uint pixels[]; } outputImage;

layout (push_constant) uniform FilterParams {
    uint imageTotalWidth;
    uint imageTotalHeight;
    uint channels;
    uint bytesPerLine;
    uint tileHeight;
    uint currentTileIndex;
    uint targetWidth;
    uint targetHeight;
    uint type;
} filterParams;

const float INV_255 = 1.0 / 255.0;
const float PI = 3.14159265359;
const float HALF_PI = 1.57079632679;
const vec4 ZERO = vec4(0.0);
const vec4 ONE = vec4(1.0);
const float EPSILON = 1e-8;

uint packColor(vec4 color) {
    uvec4 i = uvec4(clamp(color * 255.0 + 0.5, 0.0, 255.0));
    return (i.a << 24) | (i.b << 16) | (i.g << 8) | i.r;
}

vec4 unpackColor(uint color) {
    uvec4 components = uvec4(
    color & 0xFF,
    (color >> 8) & 0xFF,
    (color >> 16) & 0xFF,
    (color >> 24) & 0xFF
    );
    return vec4(components) * INV_255;
}

// 三次卷积权重计算 (Keys' Cubic Interpolation)
float cubicWeight(float d) {
    d = abs(d);
    float d2 = d * d;
    float d3 = d2 * d;

    float w = (4.0 + d3 * 3.0 - d2 * 6.0) * 0.166666667; // 1/6
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
    // 提前返回避免不必要的计算
    if (x >= 2.0) return 0.0;
    if (x < 1e-5) return 1.0;

    float pi_x = PI * x;
    float pi_x_over_2 = HALF_PI * x;

    return sin(pi_x) * sin(pi_x_over_2) / (pi_x * pi_x_over_2);
}

uint getPixel(uint localX, int localY) {
    int globalY = int(filterParams.currentTileIndex * filterParams.tileHeight) + localY;

    localX = min(localX, filterParams.imageTotalWidth - 1);
    globalY = clamp(globalY, 0, int(filterParams.imageTotalHeight) - 1);

    uint tileIdx = uint(globalY) / filterParams.tileHeight;
    uint tileLocalY = uint(globalY) % filterParams.tileHeight;
    uint pixelIdx = tileLocalY * filterParams.imageTotalWidth + localX;

    int inputBufferIdx = 4 + int(tileIdx) - int(filterParams.currentTileIndex);

    if (inputBufferIdx < 0) inputBufferIdx = 0;
    if (inputBufferIdx > 8) inputBufferIdx = 8;

    if (inputBufferIdx == 0) {
        return inputImage0.pixels[pixelIdx];
    } else if (inputBufferIdx == 1) {
        return inputImage1.pixels[pixelIdx];
    } else if (inputBufferIdx == 2) {
        return inputImage2.pixels[pixelIdx];
    } else if (inputBufferIdx == 3) {
        return inputImage3.pixels[pixelIdx];
    } else if (inputBufferIdx == 4) {
        return inputImage4.pixels[pixelIdx];
    } else if (inputBufferIdx == 5) {
        return inputImage5.pixels[pixelIdx];
    } else if (inputBufferIdx == 6) {
        return inputImage6.pixels[pixelIdx];
    } else if (inputBufferIdx == 7) {
        return inputImage7.pixels[pixelIdx];
    } else if (inputBufferIdx == 8) {
        return inputImage8.pixels[pixelIdx];
    }
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= filterParams.targetWidth || coord.y >= filterParams.targetHeight) {
        return;
    }

    uint stride = filterParams.bytesPerLine >> 2;
    vec2 targetSize = vec2(filterParams.targetWidth, filterParams.targetHeight);
    vec2 srcSize = vec2(filterParams.imageTotalWidth, 256);

    vec2 uv = (vec2(coord) + 0.5) / targetSize;
    vec2 srcCoord = uv * srcSize;

    vec2 fractPart = fract(srcCoord);
    ivec2 base = ivec2(srcCoord - fractPart);

    vec2 d_base = -fractPart + 1.0;

    vec4 finalColor = ZERO;
    float totalWeight = 0.0;

    if (filterParams.type == 1) {
        for (int y = -1; y <= 2; ++y) {
            float wy = cubicWeight(float(y) + d_base.y);

            for (int x = -1; x <= 2; ++x) {
                float wx = cubicWeight(float(x) + d_base.x);
                float weight = wx * wy;

                ivec2 samplePos = base + ivec2(x, y);
                finalColor += unpackColor(getPixel(samplePos.x, samplePos.y)) * weight;
                totalWeight += weight;
            }
        }
    } else if (filterParams.type == 2) {
        for (int y = -1; y <= 2; ++y) {
            float wy = cubicWeightNew(float(y) + d_base.y);

            for (int x = -1; x <= 2; ++x) {
                float wx = cubicWeightNew(float(x) + d_base.x);
                float weight = wx * wy;

                ivec2 samplePos = base + ivec2(x, y);
                finalColor += unpackColor(getPixel(samplePos.x, samplePos.y)) * weight;
                totalWeight += weight;
            }
        }
    } else if (filterParams.type == 3) {
        for (int y = -1; y <= 2; ++y) {
            float wy = lanczosWeight(float(y) + d_base.y);

            for (int x = -1; x <= 2; ++x) {
                float wx = lanczosWeight(float(x) + d_base.x);
                float weight = wx * wy;

                ivec2 samplePos = base + ivec2(x, y);
                finalColor += unpackColor(getPixel(samplePos.x, samplePos.y)) * weight;
                totalWeight += weight;
            }
        }
    }

    finalColor /= max(totalWeight, EPSILON);
    uint outputIndex = coord.y * filterParams.targetWidth + coord.x;
    outputImage.pixels[outputIndex] = packColor(clamp(finalColor, 0.0, 1.0));
}
