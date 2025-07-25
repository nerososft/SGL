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
    uint blockSize;
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
    // 马赛克块大小设为16x16
    const uint BLOCK_SIZE = filterParams.blockSize;
    uvec2 blockStart = (coord / BLOCK_SIZE) * BLOCK_SIZE;
    uvec2 samplePos = blockStart + BLOCK_SIZE / 2u;
    samplePos = clamp(samplePos, uvec2(0), uvec2(filterParams.width, filterParams.height) - 1u);

    // 获取块中心颜色
    vec4 color = unpackColor(inputImage.pixels[samplePos.y * filterParams.width + samplePos.x]);

    // NES颜色量化（简化处理：RGB各3bit）
    color.rgb = round(color.rgb * 7.0) / 7.0;

    // 增加对比度模拟CRT效果
    color.rgb = smoothstep(0.0, 1.0, color.rgb * 1.2 - 0.1);

    outputImage.pixels[coord.y * filterParams.width + coord.x] = packColor(color);
}
