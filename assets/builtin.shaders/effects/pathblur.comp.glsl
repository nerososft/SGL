#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer KernelBuffer {
    float vec[];
} vecData;

layout (std430, binding = 2) buffer OutputImageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform FilterParams {
    uint width;
    uint height;
    uint channels;
    uint bytesPerLine;
    int amount;
} params;

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
// 边界处理函数
int clampI(int value, int minVal, int maxVal) {
    return value < minVal ? minVal : (value > maxVal ? maxVal : value);
}
void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(params.width, params.height)))) {
        return;
    }
    uint X = gl_GlobalInvocationID.x;
    uint Y = gl_GlobalInvocationID.y;
    
    if (X >= params.width || Y >= params.height) {
        return;
    }

    float PosX_P = float(X) + 0.5;
    float PosY_P = float(Y) + 0.5;
    float PosX_N = PosX_P;
    float PosY_N = PosY_P;

    int SumB = 0;
    int SumG = 0;
    int SumR = 0;

    for (int Z = 0; Z < params.amount; Z++) {
        // 正方向向量采样
        int clampedX_P = clamp(int(floor(PosX_P)), 0, int(params.width) - 1);
        int clampedY_P = clamp(int(floor(PosY_P)), 0, int(params.height) - 1);
        int index_P = (clampedY_P * int(params.width) + clampedX_P) * 2;
        float vecX_P = vecData.vec[index_P];
        float vecY_P = vecData.vec[index_P + 1];
        PosX_P += vecX_P * 0.333333;
        PosY_P += vecY_P * 0.333333;

        // 负方向向量采样
        int clampedX_N = clamp(int(floor(PosX_N)), 0, int(params.width) - 1);
        int clampedY_N = clamp(int(floor(PosY_N)), 0, int(params.height) - 1);
        int index_N = (clampedY_N * int(params.width) + clampedX_N) * 2;
        float vecX_N = vecData.vec[index_N];
        float vecY_N = vecData.vec[index_N + 1];
        PosX_N -= vecX_N * 0.333333;
        PosY_N -= vecY_N * 0.333333;

        // 正方向颜色采样
        int sampleX_P = clamp(int(floor(PosX_P)), 0, int(params.width) - 1);
        int sampleY_P = clamp(int(floor(PosY_P)), 0, int(params.height) - 1);
        uint pixel_P = inputImage.pixels[sampleY_P * params.width + sampleX_P];
        vec4 color_P = unpackColor(pixel_P);

        // 负方向颜色采样
        int sampleX_N = clamp(int(floor(PosX_N)), 0, int(params.width) - 1);
        int sampleY_N = clamp(int(floor(PosY_N)), 0, int(params.height) - 1);
        uint pixel_N = inputImage.pixels[sampleY_N * params.width + sampleX_N];
        vec4 color_N = unpackColor(pixel_N);

        // 累加颜色值
        SumB += int(color_P.b * 255.0 + color_N.b * 255.0);
        SumG += int(color_P.g * 255.0 + color_N.g * 255.0);
        SumR += int(color_P.r * 255.0 + color_N.r * 255.0);
    }

    // 计算平均值并保留原始Alpha
    int weight = 2 * params.amount;
    vec4 originalColor = unpackColor(inputImage.pixels[Y * params.width + X]);
    vec4 filteredColor = vec4(
        float(SumR) / float(weight) / 255.0,
        float(SumG) / float(weight) / 255.0,
        float(SumB) / float(weight) / 255.0,
        originalColor.a
    );

    outputImage.pixels[Y * params.width + X] = packColor(filteredColor);
    
}