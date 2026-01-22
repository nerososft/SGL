#version 450
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageStorageBuffer0 { uint pixels[]; } inputImage[9];
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

// 预计算常数
const float INV_255 = 1.0 / 255.0;
const float PI = 3.14159265359;
const float HALF_PI = 1.57079632679;
const vec4 ZERO = vec4(0.0);
const vec4 ONE = vec4(1.0);
const float EPSILON = 1e-8; // 更小的精度值，避免除零

// 使用更高效的位操作和向量化
uint packColor(vec4 color) {
    uvec4 i = uvec4(clamp(color * 255.0 + 0.5, 0.0, 255.0));
    return (i.a << 24) | (i.b << 16) | (i.g << 8) | i.r;
}

// 使用整数运算避免浮点除法
vec4 unpackColor(uint color) {
    // 使用一次位操作和向量化转换
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
    // 使用多项式计算，避免条件分支
    float d2 = d * d;
    float d3 = d2 * d;

    // 使用step和mix替代条件分支
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

    // 预计算常用值
    float w1 = (k + 2.0) * x3 - (k + 3.0) * x2 + 1.0;
    w1 = mix(w1, 0.0, step(1.0, x));

    float w2 = k * x3 - 5.0 * k * x2 + 8.0 * k * x - 4.0 * k;
    return mix(w1, w2, step(1.0, x) * (1.0 - step(2.0, x)));
}

// Lanczos插值权重函数 (a=2.0) - 无分支优化版本
float lanczosWeight(float x) {
    x = abs(x);

    // 无分支条件判断
    float isOutside = step(2.0, x);
    float isZero = step(-EPSILON, x) * step(x, EPSILON);

    float pi_x = PI * x;
    float pi_x_over_2 = HALF_PI * x;

    float weight = sin(pi_x) * sin(pi_x_over_2) / (pi_x * pi_x_over_2);

    // 混合结果：0 (超出范围) -> weight (正常范围) -> 1 (x=0)
    weight = mix(weight, 0.0, isOutside);
    weight = mix(weight, 1.0, isZero);

    return weight;
}

// 修复后的像素获取函数
uint getPixel(uint globalX, uint globalY) {
    // 边界检查：超出图像范围返回0
    if (globalX >= filterParams.imageTotalWidth || globalY >= filterParams.imageTotalHeight) {
        return 0;
    }

    uint currentTileYStart = filterParams.tileHeight * filterParams.currentTileIndex;
    uint currentTileYEnd = currentTileYStart + filterParams.tileHeight;

    // 当前tile范围内
    if (globalY >= currentTileYStart && globalY < currentTileYEnd) {
        uint localY = globalY - currentTileYStart;
        uint localIdx = localY * filterParams.imageTotalWidth + globalX;
        return inputImage[4].pixels[localIdx];
    }

    // 上方tile
    if (globalY < currentTileYStart) {
        int yOffset = int(currentTileYStart) - int(globalY);
        int upTileCount = (yOffset + int(filterParams.tileHeight) - 1) / int(filterParams.tileHeight); // 向上取整
        int inputTileIdx = 4 - upTileCount;
        inputTileIdx = clamp(inputTileIdx, 0, 8); // 限制在0-8范围内

        // 正确计算本地Y坐标，避免负数模运算
        int tileLocalY = int(filterParams.tileHeight) - (yOffset % int(filterParams.tileHeight));
        if (tileLocalY == int(filterParams.tileHeight)) {
            tileLocalY = 0;
        }

        uint pixelIdx = uint(tileLocalY) * filterParams.imageTotalWidth + globalX;
        return inputImage[inputTileIdx].pixels[pixelIdx];
    }

    // 下方tile
    int yOffset = int(globalY) - int(currentTileYEnd);
    int downTileCount = (yOffset + int(filterParams.tileHeight) - 1) / int(filterParams.tileHeight);
    int inputTileIdx = 5 + downTileCount;
    inputTileIdx = clamp(inputTileIdx, 0, 8);

    uint tileLocalY = uint(yOffset % int(filterParams.tileHeight));
    uint pixelIdx = tileLocalY * filterParams.imageTotalWidth + globalX;
    return inputImage[inputTileIdx].pixels[pixelIdx];
}

void main() {
    uvec2 localCoord = gl_GlobalInvocationID.xy;
    uint localX = localCoord.x;
    uint localY = localCoord.y;

    // 基础边界检查：超出目标尺寸直接返回
    if (localX >= filterParams.targetWidth || localY >= filterParams.targetHeight) {
        return;
    }

    // 计算全局Y坐标
    uint globalY = filterParams.currentTileIndex * filterParams.tileHeight + localY;
    // 超出源图像高度直接返回
    if (globalY >= filterParams.imageTotalHeight) {
        return;
    }

    // 预计算常用值
    vec2 targetSize = vec2(filterParams.targetWidth, filterParams.targetHeight);
    vec2 srcSize = vec2(filterParams.imageTotalWidth, filterParams.imageTotalHeight);

    // 计算源坐标 (中心对齐)
    vec2 uv = (vec2(localCoord) + 0.5) / targetSize;
    vec2 srcCoord = uv * srcSize;

    vec2 fractPart = fract(srcCoord);
    ivec2 base = ivec2(srcCoord - fractPart);

    // 预计算相对位置偏移
    vec2 d_base = -fractPart + 1.0;

    vec4 finalColor = ZERO;
    float totalWeight = 0.0;

    // 预计算边界限制
    ivec2 sizeLimit = ivec2(filterParams.imageTotalWidth - 1, filterParams.imageTotalHeight - 1);
    if (filterParams.type == 1) {
        // 立方卷积
        for (int y = -1; y <= 2; ++y) {
            float wy = cubicWeight(float(y) + d_base.y);
            if (wy < EPSILON) continue;

            for (int x = -1; x <= 2; ++x) {
                float wx = cubicWeight(float(x) + d_base.x);
                if (wx < EPSILON) continue;

                float weight = wx * wy;

                ivec2 samplePos = base + ivec2(x, y);
                samplePos = clamp(samplePos, ivec2(0), sizeLimit);
                uint pixel = getPixel(uint(samplePos.x), uint(samplePos.y));
                finalColor += unpackColor(pixel) * weight;
                totalWeight += weight;
            }
        }
    }
    else if (filterParams.type == 2) {
        // Mitchell-Netravali
        for (int y = -1; y <= 2; ++y) {
            float wy = cubicWeightNew(float(y) + d_base.y);
            if (wy < EPSILON) continue;

            for (int x = -1; x <= 2; ++x) {
                float wx = cubicWeightNew(float(x) + d_base.x);
                if (wx < EPSILON) continue;

                float weight = wx * wy;

                ivec2 samplePos = base + ivec2(x, y);
                samplePos = clamp(samplePos, ivec2(0), sizeLimit);

                uint pixel = getPixel(uint(samplePos.x), uint(samplePos.y));
                finalColor += unpackColor(pixel) * weight;
                totalWeight += weight;
            }
        }
    }
    else if (filterParams.type == 3) {
        // Lanczos
        for (int y = -1; y <= 2; ++y) {
            float wy = lanczosWeight(float(y) + d_base.y);
            if (wy < EPSILON) continue;

            for (int x = -1; x <= 2; ++x) {
                float wx = lanczosWeight(float(x) + d_base.x);
                if (wx < EPSILON) continue;

                float weight = wx * wy;

                ivec2 samplePos = base + ivec2(x, y);
                samplePos = clamp(samplePos, ivec2(0), sizeLimit);

                uint pixel = getPixel(uint(samplePos.x), uint(samplePos.y));
                finalColor += unpackColor(pixel) * weight;
                totalWeight += weight;
            }
        }
    }

    finalColor /= max(totalWeight, EPSILON);
    uint outputIndex = localY * filterParams.targetWidth + localX;
    outputImage.pixels[outputIndex] = packColor(clamp(finalColor, 0.0, 1.0));
}