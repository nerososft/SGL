#version 450
layout (local_size_x = 512, local_size_y = 1, local_size_z = 1) in; // 1D工作组

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

const uint MAX_RADIUS = 1000;
// 共享内存声明（必须全局）
shared uint s_Pixels[512 + 2 * MAX_RADIUS];  // 核心512像素+两侧各MAX_RADIUS边界
shared float s_Weights[2 * MAX_RADIUS + 1];      // R=MAX_RADIUS时权重数组

void main() {
    const int R = filterParams.radius;
    uint width = filterParams.width;
    uint y = gl_GlobalInvocationID.y;
    ivec2 gid = ivec2(gl_GlobalInvocationID.xy);

    // 预计算权重（仅首个线程执行）
    if (gl_LocalInvocationID.x == 0) {
        float sigma = float(R) / 2; // sigma=R/2
        for (int dx = -R; dx <= R; ++dx)
        s_Weights[dx + R] = exp(-dx * dx / (2.0 * sigma * sigma));
    }

    // 协作加载共享内存（跨步加载）
    for (int i = int(gl_LocalInvocationID.x) - R; i < 512 + R; i += 512) {
        int x = clamp(int(gl_WorkGroupID.x * 512) + i, 0, int(width) - 1);
        s_Pixels[i + R] = inputImage.pixels[x + y * width];
    }

    barrier(); // 等待所有线程完成加载

    // 计算输出坐标
    uint outputX = gl_WorkGroupID.x * 512 + gl_LocalInvocationID.x;
    if (outputX >= width) return;

    // 应用高斯模糊
    vec4 sum = vec4(0);
    float wsum = 0.0;
    #pragma unroll // 展开循环提升性能
    for (int dx = -R; dx <= R; ++dx) {
        float w = s_Weights[dx + R];
        sum += unpackColor(s_Pixels[gl_LocalInvocationID.x + R + dx]) * w;
        wsum += w;
    }

    outputImage.pixels[outputX + y * width] = packColor(sum / wsum);
}