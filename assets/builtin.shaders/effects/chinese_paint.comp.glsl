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

    // 边缘检测
    float edgeStrength = 0.0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            uvec2 sampledCoord = clamp(coord + uvec2(i, j), uvec2(0), uvec2(filterParams.width - 1, filterParams.height - 1));
            vec4 c = unpackColor(inputImage.pixels[sampledCoord.y * filterParams.width + sampledCoord.x]);
            edgeStrength += dot(c.rgb, vec3(0.3, 0.6, 0.1)) * (i * j) != 0 ? 0.5 : -1.0;
        }
    }

    // 颜色简化与水墨效果
    vec4 color = unpackColor(inputImage.pixels[coord.y * filterParams.width + coord.x]);
    vec3 gray = vec3(dot(color.rgb, vec3(0.2126, 0.7152, 0.0722)));

    // 颜色量化（4级灰度）
    vec3 posterized = floor(gray * 3.0 + 0.5) / 2.0;

    // 混合边缘与颜色
    float edge = smoothstep(0.2, 0.5, abs(edgeStrength));
    vec3 finalColor = mix(posterized * 1.2, vec3(0.2), edge * 0.2);

    // 添加纸张纹理
    float noise = fract(sin(dot(coord, vec2(12.9898, 72.233))) * 43758.5453);
    finalColor += (noise - 0.5) * 0.1;
    finalColor = clamp(finalColor, 0.0, 1.0);

    outputImage.pixels[coord.y * filterParams.width + coord.x] = packColor(vec4(finalColor, color.a));
}