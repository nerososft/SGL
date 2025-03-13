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
    uint targetWidth;
    uint targetHeight;
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
    if (any(greaterThanEqual(coord, uvec2(filterParams.targetWidth, filterParams.targetHeight)))) return;

    vec2 uv = (vec2(coord) + 0.5) / vec2(filterParams.targetWidth, filterParams.targetHeight);
    vec2 srcCoord = uv * vec2(filterParams.width, filterParams.height) - 0.5;

    vec4 finalColor = vec4(0);
    float totalWeight = 0.0;

    for (int j = -1; j <= 2; ++j) {
        for (int i = -1; i <= 2; ++i) {
            vec2 samplePos = floor(srcCoord) + vec2(i, j);
            vec2 d = abs(srcCoord - samplePos);

            // Bicubic weighting function
            vec2 w = (d * d) * (3.0 - 2.0 * d);
            float weight = w.x * w.y;

            ivec2 srcPixel = ivec2(clamp(samplePos, vec2(0), vec2(filterParams.width - 1, filterParams.height - 1)));
            uint pixelIndex = srcPixel.y * (filterParams.bytesPerLine / 4) + srcPixel.x;
            vec4 color = unpackColor(inputImage.pixels[pixelIndex]);

            finalColor += color * weight;
            totalWeight += weight;
        }
    }

    finalColor /= totalWeight;
    outputImage.pixels[coord.y * filterParams.targetWidth + coord.x] = packColor(finalColor);
}
