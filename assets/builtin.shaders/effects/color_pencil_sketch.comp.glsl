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
    // TODO: filter params
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

    // 多尺度边缘检测
    float edge3x3 = 0.0, edge5x5 = 0.0;
    for (int y = -2; y <= 2; y++) {
        for (int x = -2; x <= 2; x++) {
            uvec2 pos = clamp(coord + ivec2(x, y), uvec2(0), uvec2(filterParams.width - 1));
            vec3 rgb = unpackColor(inputImage.pixels[pos.y * filterParams.width + pos.x]).rgb;
            float gray = dot(rgb, vec3(0.25, 0.65, 0.1));

            // 双尺度梯度增强
            float weight = 4.0 - length(vec2(x, y));
            edge3x3 += gray * (x + y) * weight * (abs(x) + abs(y) < 3 ? 1.0 : 0.0);
            edge5x5 += gray * (x + y) * (weight * 0.5);
        }
    }

    // 动态纹理生成
    float paper = fract(sin(dot(coord, vec2(12.9898, 78.233))) * 43758.5453) * 0.3;
    float stroke = sin(coord.x * 0.3 + coord.y * 1.7) * sin(coord.x * 1.2 - coord.y * 0.8) * 0.2;

    // 边缘合成
    float edge = clamp(abs(edge3x3) * 1.2 + abs(edge5x5) * 0.6, 0.0, 2.0);
    edge = pow(1.0 - smoothstep(0.2, 1.2, edge), 8.0) * (0.9 + paper * 0.4);

    // 色彩处理
    vec4 c = unpackColor(inputImage.pixels[coord.y * filterParams.width + coord.x]);
    vec3 quantized = floor(c.rgb * vec3(5, 7, 4) + 0.5) / vec3(4, 6, 3); // 非均匀量化

    // 最终合成
    vec3 result = mix(quantized * (0.85 + stroke),
                      vec3(1.0 - edge * 0.8),
                      clamp(edge * 1.2 - 0.1, 0.3, 0.6));

    outputImage.pixels[coord.y * filterParams.width + coord.x] =
    packColor(vec4(pow(result, vec3(1.05)), 1.0));
}