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
    int radius;
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
    vec4 colorSum = vec4(0.0);
    float weightSum = 0.0;
    float sigma = float(filterParams.radius) / 2;
    for (int dx = -filterParams.radius; dx <= filterParams.radius; ++dx) {
        int sampleX = int(coord.x) + dx;
        sampleX = clamp(sampleX, 0, int(filterParams.width) - 1);

        float weight = exp(- float(dx * dx) / (2.0 * sigma * sigma));
        vec4 sampledColor = unpackColor(inputImage.pixels[sampleX + coord.y * filterParams.width]);

        colorSum += sampledColor * weight;
        weightSum += weight;
    }

    colorSum /= weightSum;
    outputImage.pixels[coord.x + coord.y * filterParams.width] = packColor(colorSum);
}