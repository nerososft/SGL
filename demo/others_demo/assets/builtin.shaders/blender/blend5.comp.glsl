#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer BlendInputImageStorageBuffer1 {
    uint pixels[];
} blendImage1;

layout (std430, binding = 1) buffer BlendInputImageStorageBuffer2 {
    uint pixels[];
} blendImage2;

layout (std430, binding = 2) buffer BlendInputImageStorageBuffer3 {
    uint pixels[];
} blendImage3;

layout (std430, binding = 3) buffer BlendInputImageStorageBuffer4 {
    uint pixels[];
} blendImage4;

layout (std430, binding = 4) buffer BlendInputImageStorageBuffer5 {
    uint pixels[];
} blendImage5;

layout (std430, binding = 5) buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform BlenderParams {
    uint width;
    uint height;
    uint channels;
    uint bytesPerline;
} blenderParams;

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
    if (any(greaterThanEqual(coord, uvec2(blenderParams.width, blenderParams.height)))) {
        return;
    }

    uint blendIndex = coord.y * blenderParams.width + coord.x;

    vec4 blendColor1 = unpackColor(blendImage1.pixels[blendIndex]);
    vec4 blendColor2 = unpackColor(blendImage2.pixels[blendIndex]);
    vec4 blendColor3 = unpackColor(blendImage3.pixels[blendIndex]);
    vec4 blendColor4 = unpackColor(blendImage4.pixels[blendIndex]);
    vec4 blendColor5 = unpackColor(blendImage5.pixels[blendIndex]);

    vec4 mixed = blendColor1 * blendColor2 * blendColor3 * blendColor4 * blendColor5;
    mixed.a = blendColor1.a;
    outputImage.pixels[blendIndex] = packColor(mixed);
}
