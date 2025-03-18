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

vec3 applyOverlay(vec3 base, vec3 blend) {
    return vec3(
    base.r < 0.5 ? 2.0 * base.r * blend.r : 1.0 - 2.0 * (1.0 - base.r) * (1.0 - blend.r),
    base.g < 0.5 ? 2.0 * base.g * blend.g : 1.0 - 2.0 * (1.0 - base.g) * (1.0 - blend.g),
    base.b < 0.5 ? 2.0 * base.b * blend.b : 1.0 - 2.0 * (1.0 - base.b) * (1.0 - blend.b)
    );
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(blenderParams.blendImageWidth, blenderParams.blendImageHeight)))) {
        return;
    }

    uint baseIndex = (blenderParams.blendImagePosY + coord.y) * (blenderParams.baseImageBytesPerLine / 4) + (coord.x + blenderParams.blendImagePosX);
    uint blendIndex = coord.y * (blenderParams.blendImageBytesPerLine / 4) + coord.x;

    vec4 baseColor = unpackColor(baseImage.pixels[baseIndex]);
    vec4 blendColor = unpackColor(blendImage.pixels[blendIndex]);

    vec3 blendedRGB = applyOverlay(baseColor.rgb, blendColor.rgb);
    vec4 finalColor = vec4(mix(baseColor.rgb, blendedRGB, blendColor.a), baseColor.a);
    outputImage.pixels[baseIndex] = packColor(finalColor);
}