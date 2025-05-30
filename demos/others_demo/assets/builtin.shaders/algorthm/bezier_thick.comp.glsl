#version 450
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(set = 0, binding = 0) readonly buffer ControlPoints {
    vec2 bezier[100][4];
} controlPoints;

// 存储生成的曲线点的缓冲区
layout(set = 0, binding = 1) buffer CurvePoints1 {
    vec2 points[];
} curvePoints;

layout(set = 0, binding = 2) buffer PixelMap {
    uint pixels[];
} pixelMap;

// 曲线采样参数
layout(push_constant) uniform Params {
    uint lineNums;

    uint numPoints;// 生成的点数量
    float tStart;// 参数t的起始值
    float tEnd;// 参数t的结束值

    uint width;
    uint height;
} params;

// 计算三次贝塞尔曲线在参数t处的值
vec2 cubicBezier(uint line, float t) {
    // 三次贝塞尔曲线公式: B(t) = (1-t)³P₀ + 3(1-t)²tP₁ + 3(1-t)t²P₂ + t³P₃
    float u = 1.0 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    vec2 p = uuu * controlPoints.bezier[line][0];
    p += 3.0 * uu * t * controlPoints.bezier[line][1];
    p += 3.0 * u * tt * controlPoints.bezier[line][2];
    p += ttt * controlPoints.bezier[line][3];

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

void main() {
    uint idx = gl_GlobalInvocationID.x;

    if (idx >= params.numPoints){
        return;
    }

    // 计算参数t的值（在tStart和tEnd之间线性插值）
    float t = params.tStart + (params.tEnd - params.tStart) * float(idx) / float(params.numPoints - 1);

    vec2 xy[100];
    for (uint i = 0; i < params.lineNums; i++) {
        xy[i] = cubicBezier(i, t);
        curvePoints.points[idx] = xy[i];
        pixelMap.pixels[uint(floor(xy[i].y)) * params.width + uint(floor(xy[i].x))] = packColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
        pixelMap.pixels[uint(floor(xy[i].y + (params.numPoints - idx) / 20.0f)) * params.width + uint(floor(xy[i].x))] = packColor(vec4(0.0f, 1.0f, 0.0f, 1.0f));
        pixelMap.pixels[uint(floor(xy[i].y - (params.numPoints - idx) / 20.0f)) * params.width + uint(floor(xy[i].x))] = packColor(vec4(0.0f, 0.0f, 1.0f, 1.0f));
    }
}
