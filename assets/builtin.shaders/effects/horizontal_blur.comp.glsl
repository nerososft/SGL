#version 450
layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in; // 1D工作组

layout (std430, binding = 0) buffer InputImageStorageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

const uint MAX_RADIUS = 256;
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
shared uint s_Pixels[256 + 2 * MAX_RADIUS];  // 核心256像素+两侧各MAX_RADIUS边界

void main() {
    const int R = filterParams.radius;
    uint width = filterParams.width;
    uint y = gl_GlobalInvocationID.y;
    ivec2 gid = ivec2(gl_GlobalInvocationID.xy);

    // 连续加载共享内存
    for (int i = int(gl_LocalInvocationID.x) - R; i < 256 + R; i += 256) {
        int x = clamp(int(gl_WorkGroupID.x * 256) + i, 0, int(width) - 1);
        s_Pixels[i + R] = inputImage.pixels[x + y * width];
    }

    barrier();

    uint outputX = gl_WorkGroupID.x * 256 + gl_LocalInvocationID.x;
    if (outputX >= width) return;

    // 展开卷积循环
    vec4 sum = vec4(0);
    float wsum = 0.0;
    int dx = -R;
    for (; dx + 31 <= R; dx += 32) {
        #pragma unroll
        for (int n = 0; n < 32; ++n) {
            int currentDx = dx + n;
            sum += unpackColor(s_Pixels[gl_LocalInvocationID.x + R + currentDx]) * weights[currentDx + R];
            wsum += weights[currentDx + R];
        }
    }
    for (; dx <= R; ++dx) {
        float w = weights[dx + R];
        sum += unpackColor(s_Pixels[gl_LocalInvocationID.x + R + dx]) * w;
        wsum += w;
    }

    outputImage.pixels[outputX + y * width] = packColor(sum / wsum);
}