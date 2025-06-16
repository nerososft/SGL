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

layout(push_constant) uniform Params {
    uint lineNums;
    uint bodyPointsNums;
    uint assPointsNums;
    uint headPointsNums;
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

vec2 rotate90(vec2 v) {
    return vec2(-v.y, v.x);
}

vec2 rotateMinus90(vec2 v) {
    return vec2(v.y, -v.x);
}

#define PI (3.1415926)
vec2 rotateVector(vec2 v, float degrees) {
    float angleInRadians = degrees * (PI / 180.0);
    float c = cos(angleInRadians);
    float s = sin(angleInRadians);
    mat2 rotationMatrix = mat2(c, -s, s, c);
    return rotationMatrix * v;
}

float assStepAngle = 180.0f / float(params.assPointsNums);
float headStepAngle = 180.0f / float(params.headPointsNums);

void main() {
    uint idx = gl_GlobalInvocationID.x;

    if (idx >= params.bodyPointsNums){
        return;
    }

    float currentT =  float(idx) / float(params.bodyPointsNums - 1);
    float nextT =  float(idx + 1) / float(params.bodyPointsNums - 1);

    uint pointsNums = params.bodyPointsNums + params.assPointsNums + params.headPointsNums;

    for (uint lineIdx = 0; lineIdx < params.lineNums; lineIdx++) {
        vec2 pointCurrent;
        vec2 pointNext;
        vec2 pointUp;
        vec2 pointDown;
        pointCurrent = cubicBezier(lineIdx, currentT);
        pointNext = cubicBezier(lineIdx, nextT);

        float thinkness = inputLines.bezier[lineIdx].beginWidth - (inputLines.bezier[lineIdx].beginWidth - inputLines.bezier[lineIdx].endWidth) * currentT;
        vec2 dir = normalize(pointNext - pointCurrent) * thinkness;

        pointUp = pointCurrent + rotate90(dir);
        pointDown = pointCurrent + rotateMinus90(dir);

        uint upOffset = lineIdx * pointsNums * 2;
        uint downEnd = upOffset + 2 * pointsNums;

        if (idx == 0) {
            for (uint assIndex = 0; assIndex < params.assPointsNums; assIndex++) {
                vec2 pointAss = pointCurrent + rotateVector(dir, 90.0f + assIndex * assStepAngle);
                outputPoints.points[upOffset + assIndex] = pointAss;
            }
        }

        if (idx == params.bodyPointsNums - 1) {
            uint bodyHeadNums  = params.headPointsNums + params.bodyPointsNums;
            for (uint headIndex = 0; headIndex < params.headPointsNums; headIndex++) {
                vec2 pointHead = pointCurrent + rotateVector(dir, -90.0f + headIndex * headStepAngle);
                outputPoints.points[downEnd - bodyHeadNums + headIndex] = pointHead;
            }
        }

        outputPoints.points[upOffset + params.assPointsNums + idx] = pointUp;
        outputPoints.points[downEnd - idx - 1] = pointDown;
    }
}
