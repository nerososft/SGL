// 版本450核心
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

    float blenderFactor;
    float brightnessThreshold;  // 新增亮度阈值参数
} blenderParams;

// ABGR格式打包/解包函数（保持与现有代码一致）
uint packColor(vec4 color) {
    return (uint(clamp(color.a, 0.0, 1.0) * 255.0) << 24) |
    (uint(clamp(color.b, 0.0, 1.0) * 255.0) << 16) |
    (uint(clamp(color.g, 0.0, 1.0) * 255.0) << 8) |
    uint(clamp(color.r, 0.0, 1.0) * 255.0);
}

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

    // 超出混合图像范围直接返回
    if (coord.x > blenderParams.blendImageWidth ||
        coord.y > blenderParams.blendImageHeight) {
        return;
    }

    // 计算基础图像和混合图像的索引
    uint baseIndex = (blenderParams.blendImagePosY + coord.y) *
                     (blenderParams.baseImageBytesPerLine / 4) +
                     (coord.x + blenderParams.blendImagePosX);

    uint blendIndex = coord.y *
                     (blenderParams.blendImageBytesPerLine / 4) +
                     coord.x;

    // 获取颜色值（ABGR格式）
    vec4 baseColor = unpackColor(baseImage.pixels[baseIndex]);
    vec4 blendColor = unpackColor(blendImage.pixels[blendIndex]);

    // 计算亮度比较（保持与CPU代码一致的逻辑）
    float baseBrightness = baseColor.r + baseColor.g + baseColor.b;
    float blendBrightness = blendColor.r + blendColor.g + blendColor.b;

    // 亮度混合逻辑
    vec4 finalColor = baseColor;
    if (blendBrightness > baseBrightness) {
        finalColor = blendColor;
    }

    // 应用混合因子（可选透明度混合）
    finalColor.a = mix(finalColor.a, blendColor.a, blenderParams.blenderFactor);

    outputImage.pixels[baseIndex] = packColor(finalColor);
}