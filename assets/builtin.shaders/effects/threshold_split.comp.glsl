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
    int bright; // 阈值
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

    uint index = coord.y * (filterParams.bytesPerLine / 4) + coord.x;
    vec4 color = unpackColor(inputImage.pixels[index]);
    // 计算灰度值
    float gray = 0.2989 * color.r + 0.5870 * color.g + 0.1140 * color.b;

    // 根据阈值进行二值化
    if (gray >= float(filterParams.bright) / 255.0) {
        // 大于等于阈值，设置为白色
        color = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
        // 小于阈值，设置为黑色
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }

    // 将结果写回输出缓冲区
    outputImage.pixels[index] = packColor(color);
}