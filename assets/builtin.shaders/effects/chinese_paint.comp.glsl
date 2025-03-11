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

    // 原始颜色读取与去色
    vec4 color = unpackColor(inputImage.pixels[coord.y * filterParams.width + coord.x]);
    float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));

    // 强化边缘检测（标准Sobel算子）
    float sobelX[9] = float[](-1, 0, 1, -2, 0, 2, -1, 0, 1);
    float sobelY[9] = float[](-1, -2, -1, 0, 0, 0, 1, 2, 1);
    float gx = 0.0, gy = 0.0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            uvec2 p = clamp(coord + uvec2(i, j), uvec2(0), uvec2(filterParams.width, filterParams.height) - 1u);
            float ngray = dot(unpackColor(inputImage.pixels[p.y * filterParams.width + p.x]).rgb,
                              vec3(0.299, 0.587, 0.114));
            int idx = (j + 1) * 3 + (i + 1);
            gx += ngray * sobelX[idx];
            gy += ngray * sobelY[idx];
        }
    }
    float edge = 1.0 - clamp(length(vec2(gx, gy)) * 2.0, 0.0, 1.0);
    edge = pow(edge, 2.5); // 强化墨线对比度

    // 5x5高斯模糊（σ=2.0）
    float blurred = 0.0;
    float kernel[25] = float[](
    0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
    0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
    0.023792, 0.094907, 0.150342, 0.094907, 0.023792,
    0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
    0.003765, 0.015019, 0.023792, 0.015019, 0.003765
    );
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            uvec2 p = clamp(coord + uvec2(i, j), uvec2(0), uvec2(filterParams.width, filterParams.height) - 1u);
            float ngray = dot(unpackColor(inputImage.pixels[p.y * filterParams.width + p.x]).rgb,
                              vec3(0.299, 0.587, 0.114));
            blurred += ngray * kernel[(j + 2) * 5 + (i + 2)];
        }
    }

    // 墨色分层与纹理混合
    float finalColor = mix(edge, blurred, 0.8);
    finalColor = floor(finalColor * 7.0) / 7.0; // 7阶墨色

    // 分形宣纸纹理（简化实现）
    vec2 uv = vec2(coord) / vec2(filterParams.width, filterParams.height);
    float noise1 = fract(sin(dot(uv * 50.0, vec2(12.9898, 78.233))) * 43758.5453);
    float noise2 = fract(sin(dot(uv * 200.0, vec2(4.898, 7.23))) * 23421.631);
    finalColor *= 1.0 - (noise1 * 0.3 + noise2 * 0.7) * 0.25;

    // 青黛色调混合
    vec3 inkColor = mix(vec3(0.08, 0.12, 0.15), color.rgb * 0.4, smoothstep(0.3, 0.7, finalColor));
    vec4 outColor = vec4(inkColor * finalColor * 0.95, color.a);

    outputImage.pixels[coord.y * filterParams.width + coord.x] = packColor(outColor);
}