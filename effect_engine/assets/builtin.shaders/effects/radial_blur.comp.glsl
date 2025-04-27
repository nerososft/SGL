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
    float strength;
    float rotationStrength;
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
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) return;

    vec2 center = vec2(filterParams.center);
    vec2 dir = center - vec2(coord);
    float distance = length(dir);

    // 旋转方向向量（切线方向）
    vec2 rotDir = normalize(vec2(dir.y, -dir.x)) * filterParams.rotationStrength * (1.0 - exp(-distance * filterParams.sharpness));
    // 径向方向向量
    vec2 radialDir = normalize(dir) * filterParams.strength * 0.5 * (1.0 - exp(-distance * filterParams.sharpness));

    vec4 colorSum = unpackColor(inputImage.pixels[coord.y * (filterParams.bytesPerLine / 4) + coord.x]);
    float weightSum = 1.0;

    const int SAMPLES = 20;
    for (int i = 1; i <= SAMPLES; ++i) {
        float t = float(i) / SAMPLES;
        // 混合旋转和径向方向
        vec2 blendedDir = mix(radialDir, rotDir, t);
        vec2 samplePos = vec2(coord) + blendedDir * t;

        ivec2 isample = ivec2(clamp(samplePos, vec2(0), vec2(filterParams.width - 1, filterParams.height - 1)));
        uint idx = isample.y * (filterParams.bytesPerLine / 4) + isample.x;

        float weight = 1.0 - t;
        colorSum += unpackColor(inputImage.pixels[idx]) * weight;
        weightSum += weight;
    }

    outputImage.pixels[coord.y * (filterParams.bytesPerLine / 4) + coord.x] =
    packColor(colorSum / weightSum);
}