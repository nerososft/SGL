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
    float redFactor;
    float greenFactor;
    float blueFactor;
} filterParams;

// ARGB
uint packColor(vec4 color) {
    return (uint(clamp(color.a, 0.0, 1.0) * 255.0) << 24) |
    (uint(clamp(color.r, 0.0, 1.0) * 255.0) << 16) |
    (uint(clamp(color.g, 0.0, 1.0) * 255.0) << 8) |
    (uint(clamp(color.b, 0.0, 1.0) * 255.0));
}

// ARGB
vec4 unpackColor(uint color) {
    return vec4(float((color >> 24) & 0xFF) / 255.0f,
    float((color >> 16) & 0xFF) / 255.0f,
    float((color >> 8) & 0xFF) / 255.0f,
    float((color) & 0xFF) / 255.0f);
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) {
        return;
    }

    uint index = coord.y * filterParams.width + coord.x;
    uint index = coord.y * (filterParams.bytesPerLine / 4) + coord.x;
    vec4 color = unpackColor(inputImage.pixels[index]);
    float gray = dot(color.rgb, vec3(filterParams.redFactor, filterParams.greenFactor, filterParams.blueFactor));
    outputImage.pixels[index] = packColor(vec4(vec3(gray), color.a));
}