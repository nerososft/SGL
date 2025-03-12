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

    // 增强颜色采样
    vec4 color = unpackColor(inputImage.pixels[coord.y * filterParams.bytesPerLine / 4 + coord.x]);
    vec3 luminance = vec3(0.299, 0.587, 0.114);
    float brightness = dot(color.rgb, luminance);

    // 改进的Sobel边缘检测
    float Gx = 0.0, Gy = 0.0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            ivec2 samplePos = ivec2(coord) + ivec2(x, y);
            if (any(lessThan(samplePos, ivec2(0))) || any(greaterThanEqual(samplePos, ivec2(filterParams.width, filterParams.height))))
            continue;

            vec4 sampleColor = unpackColor(inputImage.pixels[samplePos.y * filterParams.bytesPerLine / 4 + samplePos.x]);
            float intensity = dot(sampleColor.rgb, luminance);

            Gx += intensity * (x == 0 ? 2 * x : x); // Sobel X核 [-1 0 1; -2 0 2; -1 0 1]
            Gy += intensity * (y == 0 ? 2 * y : y); // Sobel Y核 [-1 -2 -1; 0 0 0; 1 2 1]
        }
    }
    float edge = clamp(1.0 - length(vec2(Gx, Gy)) * 2.5, 0.0, 1.0);

    // 动态铅笔纹理生成
    float stroke = fract(coord.x * 0.3 + coord.y * 0.7 + sin(coord.y * 0.1) * 2.0);
    stroke = smoothstep(0.3, 0.8, stroke * brightness) * 1.2;

    // 颜色增强混合
    vec3 pencilColor = color.rgb * (1.0 - 0.3 * edge) * stroke;
    vec3 finalColor = mix(pencilColor, vec3(0.4, 0.3, 0.2), pow(edge, 2.0)); // 添加纸张色调

    outputImage.pixels[coord.y * filterParams.bytesPerLine / 4 + coord.x] =
    packColor(vec4(pow(finalColor, vec3(0.92)), color.a * (0.8 + 0.2 * edge)));
}