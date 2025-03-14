#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer BaseInputImageStorageBuffer {
    uint pixels[];
} baseImage;

layout (std430, binding = 1) buffer BlendInputImageStorageBuffer {
    uint pixels[];
} blendImage;

layout (std430, binding = 2) buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform BlenderParams {
    uint baseImageWidth;
    uint baseImageHeight;
    uint baseImageChannels;
    uint baseImageBytesPerLine;

    uint blendImageWidth;
    uint blendImageHeight;
    uint blendImageChannels;
    uint blendImageBytesPerLine;
    uint blendImagePosX;
    uint blendImagePosY;

    // TODO: blend params
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
    if (any(greaterThanEqual(coord, uvec2(blenderParams.blendImagePosX, blenderParams.blendImagePosY)))) {
        return;
    }
    // TODO: blend implement
}
