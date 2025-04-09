#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer posx {
    float px[];
} posxData;
layout (std430, binding = 2) buffer posy {
    float py[];
} posyData;
layout (std430, binding = 3) buffer OutputImageBuffer {
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

// 像素坐标转换函数
uint getPixelIndex(uint x, uint y) {
    return y * (params.bytesPerLine / 4) + x;
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= params.width || coord.y >= params.height) return;

    // 当前像素坐标
    vec2 currentPos = vec2(coord);
    float minDist = 3.4028235e38;// FLT_MAX
    vec2 closestPoint = vec2(0);

    // 遍历所有控制点
    for (int i = 0; i < params.amount; ++i) {
        if (i >= params.amount) break;

        // 读取控制点坐标
        vec2 samplePoint = vec2(posxData.px[i], posyData.py[i]);

        // 计算欧氏距离平方
        vec2 diff = currentPos - samplePoint;
        float dist = dot(diff, diff);

        // 更新最近点
        if (dist < minDist) {
            minDist = dist;
            closestPoint = samplePoint;
        }
    }

    // 计算采样坐标
    ivec2 srcCoord = ivec2(
    clamp(int(round(closestPoint.x)), 0, int(params.width)-1),
    clamp(int(round(closestPoint.y)), 0, int(params.height)-1)
    );

    // 读取源像素颜色
    uint srcIndex = srcCoord.y * (params.bytesPerLine/4) + srcCoord.x;
    vec4 color = unpackColor(inputImage.pixels[srcIndex]);

    // 保持原始Alpha通道（可选）
    // vec4 origColor = unpackColor(inputImage.pixels[coord.y*(params.bytesPerLine/4)+coord.x]);
    // color.a = origColor.a;

    // 写入输出
    uint outIndex = coord.y * (params.bytesPerLine/4) + coord.x;
    outputImage.pixels[outIndex] = packColor(color);
}