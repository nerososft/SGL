#version 450

layout (local_size_x = 1, local_size_y = 512, local_size_z = 1) in;

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

const uint MAX_RADIUS = 1000;
// 共享内存声明（必须全局）
shared uint s_Pixels[512 + 2 * MAX_RADIUS];  // 核心256像素+两侧各MAX_RADIUS边界
shared float s_Weights[2 * MAX_RADIUS + 1];      // R=MAX_RADIUS时权重数组

void main() {
    const int R = filterParams.radius;
    uint height = filterParams.height;
    ivec2 gid = ivec2(gl_GlobalInvocationID.xy);

    // 预计算权重（仅首个线程）
    if (gl_LocalInvocationID.y == 0) {
        float sigma = float(R) / 2; // sigma=R/2
        for (int dy = -R; dy <= R; ++dy)
        s_Weights[dy + R] = exp(-dy * dy / (2.0 * sigma * sigma));
    }

    // 垂直方向数据加载
    for (int j = int(gl_LocalInvocationID.y) - R; j < 512 + R; j += 512) {
        int y = clamp(int(gl_WorkGroupID.y * 512) + j, 0, int(height) - 1);
        s_Pixels[j + R] = inputImage.pixels[gid.x + y * filterParams.width];
    }

    barrier();

    // 计算输出坐标
    uint outputY = gl_WorkGroupID.y * 512 + gl_LocalInvocationID.y;
    if (outputY >= height) return;

    // 垂直卷积
    vec4 sum = vec4(0);
    float wsum = 0.0;
    #pragma unroll
    for (int dy = -R; dy <= R; ++dy) {
        float w = s_Weights[dy + R];
        sum += unpackColor(s_Pixels[gl_LocalInvocationID.y + R + dy]) * w;
        wsum += w;
    }

    outputImage.pixels[gid.x + outputY * filterParams.width] = packColor(sum / wsum);
}
