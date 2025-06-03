#version 450
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct BezierLine {
    vec2 points[4];
    float beginWidth;
    float endWidth;
};

layout(set = 0, binding = 0) readonly buffer InputLines {
    BezierLine bezier[];
} inputLines;

layout(set = 0, binding = 1) buffer ThinkLineOutputPoints {
    vec2 points[];
} outputPoints;

layout(set = 0, binding = 2) buffer PixelMap {
    uint pixels[];
} pixelMap;

layout(push_constant) uniform Params {
    uint lineNums;
    uint numPoints;// 生成的点数量
    bool debugPixelMap;
} params;

vec2 cubicBezier(uint line, float t) {
    // 三次贝塞尔曲线公式: B(t) = (1-t)³P₀ + 3(1-t)²tP₁ + 3(1-t)t²P₂ + t³P₃
    float u = 1.0 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    vec2 p = uuu * inputLines.bezier[line].points[0];
    p += 3.0 * uu * t * inputLines.bezier[line].points[1];
    p += 3.0 * u * tt * inputLines.bezier[line].points[2];
    p += ttt * inputLines.bezier[line].points[3];

    return p;
}

uint packColor(vec4 color) {
    return (
    uint(clamp(color.a, 0.0, 1.0) * 255.0) << 24) |
    (uint(clamp(color.b, 0.0, 1.0) * 255.0) << 16) |
    (uint(clamp(color.g, 0.0, 1.0) * 255.0) << 8) |
    (uint(clamp(color.r, 0.0, 1.0) * 255.0)
    );
}

#define MAX_LINE_NUM (1024)

void main() {
    uint idx = gl_GlobalInvocationID.x;

    if (idx >= params.numPoints){
        return;
    }

    float t =  float(idx) / float(params.numPoints);

    vec2 point[MAX_LINE_NUM];
    vec2 pointUp[MAX_LINE_NUM];
    vec2 pointDown[MAX_LINE_NUM];
    for (uint lineIdx = 0; lineIdx < params.lineNums; lineIdx++) {
        point[lineIdx] = cubicBezier(lineIdx, t);

        float thinkness = inputLines.bezier[lineIdx].beginWidth - (inputLines.bezier[lineIdx].beginWidth - inputLines.bezier[lineIdx].endWidth) * t;
        pointUp[lineIdx] = vec2(point[lineIdx].x, point[lineIdx].y + thinkness);
        pointDown[lineIdx] = vec2(point[lineIdx].x, point[lineIdx].y - thinkness);

        uint offset = lineIdx * params.numPoints * 2;
        outputPoints.points[offset + lineIdx] = pointUp[lineIdx];
        outputPoints.points[offset + params.numPoints + lineIdx] = pointDown[lineIdx];

        if (params.debugPixelMap) {
            pixelMap.pixels[uint(floor(point[lineIdx].y)) * params.numPoints + uint(floor(point[lineIdx].x))] = packColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
            pixelMap.pixels[uint(floor(pointUp[lineIdx].y)) * params.numPoints + uint(floor(pointUp[lineIdx].x))] = packColor(vec4(0.0f, 1.0f, 0.0f, 1.0f));
            pixelMap.pixels[uint(floor(pointDown[lineIdx].y)) * params.numPoints + uint(floor(pointDown[lineIdx].x))] = packColor(vec4(0.0f, 0.0f, 1.0f, 1.0f));
        }
    }
}
