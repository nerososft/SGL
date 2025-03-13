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
    float redOffsetX;
    float greenOffsetX;
    float blueOffsetX;
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

    // 定义各通道水平偏移量（可调整）
    const float RED_OFFSET = filterParams.redOffsetX;
    const float GREEN_OFFSET = filterParams.greenOffsetX;
    const float BLUE_OFFSET = filterParams.blueOffsetX;

    // 计算各通道采样坐标（带边界约束）
    float redX = clamp(coord.x + RED_OFFSET, 0, filterParams.width - 1);
    float greenX = clamp(coord.x + GREEN_OFFSET, 0, filterParams.width - 1);
    float blueX = clamp(coord.x + BLUE_OFFSET, 0, filterParams.width - 1);

    // 读取各通道颜色值
    vec4 red = unpackColor(inputImage.pixels[coord.y * filterParams.width + int(redX)]);
    vec4 green = unpackColor(inputImage.pixels[coord.y * filterParams.width + int(greenX)]);
    vec4 blue = unpackColor(inputImage.pixels[coord.y * filterParams.width + int(blueX)]);

    // 获取原始alpha通道
    vec4 original = unpackColor(inputImage.pixels[coord.y * filterParams.width + coord.x]);

    // 合成新颜色并写入输出
    outputImage.pixels[coord.y * filterParams.width + coord.x] =
    packColor(vec4(red.r, green.g, blue.b, original.a));
}