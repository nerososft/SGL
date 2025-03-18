#version 450
layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in; // 1D工作组

layout (std430, binding = 0) buffer InputImageStorageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

const uint MAX_RADIUS = 512;
layout (binding = 2) uniform WeightUBO {
    float weights[2 * MAX_RADIUS + 1];
};

layout (push_constant) uniform FilterParams {
    uint width;
    uint height;
    uint channels;
    uint bytesPerLine;
    int radius;
} filterParams;

// ABGR
uint packColor(vec4 color) {
    return (uint(clamp(color.a, 0.0, 1.0) * 255.0) << 24) |
    (uint(clamp(color.b, 0.0, 1.0) * 255.0) << 16) |
    (uint(clamp(color.g, 0.0, 1.0) * 255.0) << 8) |
    (uint(clamp(color.r, 0.0, 1.0) * 255.0));
}

// ABGR
vec4 unpackColor(uint color) {
    return vec4(float((color) & 0xFF) / 255.0f,
    float((color >> 8) & 0xFF) / 255.0f,
    float((color >> 16) & 0xFF) / 255.0f,
    float((color >> 24) & 0xFF) / 255.0f);
}

// 共享内存声明（必须全局）
shared vec4 s_Pixels[256 + 2 * MAX_RADIUS];  // 核心256像素+两侧各MAX_RADIUS边界

void main() {
    const int R = filterParams.radius;
    uint width = filterParams.width;
    uint y = gl_GlobalInvocationID.y;
    ivec2 gid = ivec2(gl_GlobalInvocationID.xy);

    // 连续加载共享内存
    uint totalElements = 256 + 2 * R;
    uint elementsPerThread = (totalElements + 255) / 256;
    for (uint j = 0; j < elementsPerThread; ++j) {
        uint i = gl_LocalInvocationID.x * elementsPerThread + j;
        if (i >= totalElements) break;
        int x = int(gl_WorkGroupID.x * 256) + int(i) - R;
        x = clamp(x, 0, int(width) - 1);
        s_Pixels[i] = unpackColor(inputImage.pixels[x + y * width]);
    }

    barrier();

    uint outputX = gl_WorkGroupID.x * 256 + gl_LocalInvocationID.x;
    if (outputX >= width) return;

    // 展开卷积循环
    vec4 sum = vec4(0);
    float wsum = 0.0;
    int dx = -R;
    for (; dx + 15 <= R; dx += 16) {
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx] * weights[dx + R];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 1] * weights[dx + R + 1];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 2] * weights[dx + R + 2];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 3] * weights[dx + R + 3];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 4] * weights[dx + R + 4];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 5] * weights[dx + R + 5];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 6] * weights[dx + R + 6];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 7] * weights[dx + R + 7];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 8] * weights[dx + R + 8];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 9] * weights[dx + R + 9];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 0] * weights[dx + R + 10];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 11] * weights[dx + R + 11];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 12] * weights[dx + R + 12];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 13] * weights[dx + R + 13];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 14] * weights[dx + R + 14];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx + 15] * weights[dx + R + 15];
        wsum += weights[dx + R]
        + weights[dx + R + 1]
        + weights[dx + R + 2]
        + weights[dx + R + 3]
        + weights[dx + R + 4]
        + weights[dx + R + 5]
        + weights[dx + R + 6]
        + weights[dx + R + 7]
        + weights[dx + R + 8]
        + weights[dx + R + 9]
        + weights[dx + R + 10]
        + weights[dx + R + 11]
        + weights[dx + R + 12]
        + weights[dx + R + 13]
        + weights[dx + R + 14]
        + weights[dx + R + 15];
    }
    for (; dx <= R; ++dx) {
        float w = weights[dx + R];
        sum += s_Pixels[gl_LocalInvocationID.x + R + dx] * w;
        wsum += w;
    }

    outputImage.pixels[outputX + y * width] = packColor(sum / wsum);
}