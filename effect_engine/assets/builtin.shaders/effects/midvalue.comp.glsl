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
    uint piece;
    uint pieceCount;
    float radius;
    float threshold;
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
uvec4 unpackColor3u(uint color) {
    return uvec4((color) & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF, (color >> 24) & 0xFF);
}
#define MAX_HISTOGRAM_SIZE 256 // 8-bit颜色通道
// 计算中值函数
int findMedian(int histogram[MAX_HISTOGRAM_SIZE], int total) {
    int medianPos = total / 2;
    int sum = 0;
    for (int i=0; i<MAX_HISTOGRAM_SIZE; ++i) {
        sum += histogram[i];
        if (sum > medianPos) return i;
    }
    return MAX_HISTOGRAM_SIZE-1;
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= filterParams.width || coord.y >= filterParams.height) return;

    // 初始化直方图
    int histR[MAX_HISTOGRAM_SIZE], histG[MAX_HISTOGRAM_SIZE], histB[MAX_HISTOGRAM_SIZE];
    for (int i=0; i<MAX_HISTOGRAM_SIZE; ++i) {
        histR[i] = histG[i] = histB[i] = 0;
    }

    // 计算采样参数
    int radius = int(round(filterParams.radius));
    ivec2 center = ivec2(coord);
    int count = 0;

    // 收集样本并构建直方图
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            ivec2 samplePos = ivec2(
            clamp(center.x + x, 0, int(filterParams.width-1)),
            clamp(center.y + y, 0, int(filterParams.height-1))
            );

            uint idx = samplePos.y * (filterParams.bytesPerLine/4) + samplePos.x;
            vec4 color = unpackColor(inputImage.pixels[idx]);

            // 转换为8-bit整数并统计
            int r = int(round(color.r * 255.0));
            int g = int(round(color.g * 255.0));
            int b = int(round(color.b * 255.0));

            histR[r]++;
            histG[g]++;
            histB[b]++;
            count++;
        }
    }


    // 获取各通道中值
    float medianR = float(findMedian(histR, count)) / 255.0;
    float medianG = float(findMedian(histG, count)) / 255.0;
    float medianB = float(findMedian(histB, count)) / 255.0;

    // 保持原始Alpha通道
    vec4 origColor = unpackColor(inputImage.pixels[center.y*(filterParams.bytesPerLine/4)+center.x]);

    // 合成最终颜色
    vec4 result = vec4(medianR, medianG, medianB, origColor.a);

    // 写入输出
    uint outIdx = coord.y * (filterParams.bytesPerLine/4) + coord.x;
    outputImage.pixels[outIdx] = packColor(result);


}

