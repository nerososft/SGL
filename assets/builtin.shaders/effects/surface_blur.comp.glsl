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
    uint blurRadius;
    float threshold; // Normalized threshold (0.0 - 1.0)
} filterParams;

uint packColor(vec4 color) {
    return (
    uint(clamp(color.a, 0.0, 1.0) * 255.0) << 24) |
    (uint(clamp(color.b, 0.0, 1.0) * 255.0) << 16) |
    (uint(clamp(color.g, 0.0, 1.0) * 255.0) << 8) |
    uint(clamp(color.r, 0.0, 1.0) * 255.0);
}

vec4 unpackColor(uint color) {
    return vec4(
    float(color & 0xFF) / 255.0,
    float((color >> 8) & 0xFF) / 255.0,
    float((color >> 16) & 0xFF) / 255.0,
    float((color >> 24) & 0xFF) / 255.0
    );
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) {
        return;
    }

    uint centerIndex = coord.y * (filterParams.bytesPerLine / 4) + coord.x;
    vec4 centerColor = unpackColor(inputImage.pixels[centerIndex]);
    vec4 sum = vec4(0.0);
    float totalWeight = 0.0;
    int radius = int(filterParams.blurRadius);

    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            ivec2 sampleCoord = ivec2(coord) + ivec2(x, y);
            if (all(greaterThanEqual(sampleCoord, ivec2(0))) &&
            all(lessThan(sampleCoord, ivec2(filterParams.width, filterParams.height)))) {
                uint index = uint(sampleCoord.y) * (filterParams.bytesPerLine / 4) + uint(sampleCoord.x);
                vec4 sampleColor = unpackColor(inputImage.pixels[index]);

                // Calculate color difference (max channel difference)
                float dr = abs(centerColor.r - sampleColor.r);
                float dg = abs(centerColor.g - sampleColor.g);
                float db = abs(centerColor.b - sampleColor.b);
                float diff = max(max(dr, dg), db);

                // Calculate weight based on threshold
                float weight = max(0.0, 1.0 - diff / filterParams.threshold);
                sum += sampleColor * weight;
                totalWeight += weight;
            }
        }
    }

    vec4 resultColor = totalWeight > 0.0 ? sum / totalWeight : centerColor;
    outputImage.pixels[centerIndex] = packColor(resultColor);
}