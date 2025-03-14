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

layout (push_constant) uniform FilterParams {
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
    if (any(greaterThanEqual(coord, uvec2(filterParams.baseImageWidth, filterParams.baseImageHeight)))) {
        return;
    }

    // 计算base图像索引
    uint baseIndex = coord.y * (filterParams.baseImageBytesPerLine / 4) + coord.x;
    vec4 baseColor = unpackColor(baseImage.pixels[baseIndex]);
    vec4 finalColor = baseColor;

    // 检查是否在混合区域
    uvec2 blendPos = uvec2(filterParams.blendImagePosX, filterParams.blendImagePosY);
    if (coord.x >= blendPos.x && coord.y >= blendPos.y &&
    coord.x < blendPos.x + filterParams.blendImageWidth &&
    coord.y < blendPos.y + filterParams.blendImageHeight) {

        // 计算blend图像索引
        uvec2 blendCoord = coord - blendPos;
        uint blendIndex = blendCoord.y * (filterParams.blendImageBytesPerLine / 4) + blendCoord.x;
        vec4 blendColor = unpackColor(blendImage.pixels[blendIndex]);

        // 生成随机数（简单哈希函数）
        uint seed = (coord.x << 16) | coord.y;
        seed = seed * 747796405u + 2891336453u;
        seed = (seed >> 16) ^ seed;
        float rand = float(seed % 1000u) / 1000.0f;

        // 溶解混合（假设存在dissolveThreshold参数）
        if (rand < 0.5f) { // 实际应从filterParams获取阈值
                           finalColor = blendColor;
        }
    }

    // 写入输出
    outputImage.pixels[baseIndex] = packColor(finalColor);
}