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
    float threshold;
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
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) return;

    const uint pixelsPerLine = filterParams.bytesPerLine / 4;
    const uint centerIdx = coord.y * pixelsPerLine + coord.x;
    const vec4 centerCol = unpackColor(inputImage.pixels[centerIdx]);
    const ivec2 iCoord = ivec2(coord);
    const int radius = int(filterParams.blurRadius);

    vec4 sum = vec4(0);
    float totalWeight = 0.0;

    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            ivec2 sampleCoord = clamp(iCoord + ivec2(x, y), ivec2(0), ivec2(filterParams.width - 1, filterParams.height - 1));
            vec4 sampleCol = unpackColor(inputImage.pixels[sampleCoord.y * pixelsPerLine + sampleCoord.x]);

            float diff = max(max(abs(centerCol.r - sampleCol.r),
                                 abs(centerCol.g - sampleCol.g)), abs(centerCol.b - sampleCol.b));

            if (diff < filterParams.threshold) {
                float weight = 1.0 - diff / filterParams.threshold;
                sum += sampleCol * weight;
                totalWeight += weight;
            }
        }
    }

    outputImage.pixels[centerIdx] = packColor(totalWeight > 0.0 ? sum / totalWeight : centerCol);
}