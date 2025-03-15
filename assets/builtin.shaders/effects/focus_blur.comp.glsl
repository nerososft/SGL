#version 450

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

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
    uvec2 center;
    uint blurRadius;
    uint centerRadius;
    float sharpness;
} filterParams;

// ABGR
uint packColor(vec4 color) {
    return (
    uint(clamp(color.a, 0.0, 1.0) * 255.0) << 24) |
    (uint(clamp(color.b, 0.0, 1.0) * 255.0) << 16) |
    (uint(clamp(color.g, 0.0, 1.0) * 255.0) << 8) |
    (uint(clamp(color.r, 0.0, 1.0) * 255.0)
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

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) {
        return;
    }

    // 计算当前像素到中心的距离
    vec2 center = vec2(filterParams.center);
    float dist = distance(vec2(coord), center);

    // 获取原始颜色
    uint pixelIndex = coord.y * (filterParams.bytesPerLine / 4) + coord.x;
    vec4 originalColor = unpackColor(inputImage.pixels[pixelIndex]);
    vec4 finalColor = originalColor;

    // 仅在模糊区域外处理
    if (dist > filterParams.centerRadius) {
        // 计算模糊混合因子
        float t = clamp((dist - filterParams.centerRadius) /
                        float(filterParams.blurRadius - filterParams.centerRadius), 0.0, 1.0);
        float blendFactor = pow(1.0 - t, filterParams.sharpness);

        // 均值模糊采样
        vec4 blurSum = vec4(0);
        int sampleCount = 0;
        int radius = int(filterParams.blurRadius);

        for (int y = -radius; y <= radius; ++y) {
            for (int x = -radius; x <= radius; ++x) {
                ivec2 sampleCoord = ivec2(coord) + ivec2(x, y);
                if (all(greaterThanEqual(sampleCoord, ivec2(0))) &&
                all(lessThan(sampleCoord, ivec2(filterParams.width, filterParams.height)))) {
                    uint index = uint(sampleCoord.y) * (filterParams.bytesPerLine / 4) + uint(sampleCoord.x);
                    blurSum += unpackColor(inputImage.pixels[index]);
                    sampleCount++;
                }
            }
        }

        vec4 blurColor = blurSum / float(sampleCount);
        finalColor = mix(blurColor, originalColor, blendFactor);
    }

    outputImage.pixels[pixelIndex] = packColor(finalColor);
}