#version 450

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer PointData {
    vec4 controlPoints[4];
    int numSegments;
} pointData;

layout (std430, binding = 1) buffer OutputData {
    vec4 points[];
} outputData;

layout (push_constant) uniform Params {
    uint width;
// TODO:
} params;

vec4 calculateBezierPoint(vec4 P0, vec4 P1, vec4 P2, vec4 P3, float t) {
    return (1.0 - t) * (1.0 - t) * (1.0 - t) * P0 +
    3.0 * (1.0 - t) * (1.0 - t) * t * P1 +
    3.0 * (1.0 - t) * t * t * P2 +
    t * t * t * P3;
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    float t = float(coord.x) / float(pointData.numSegments);

    // Calculate the point on the Bezier curve
    vec4 bezierPoint = calculateBezierPoint(
    pointData.controlPoints[0],
    pointData.controlPoints[1],
    pointData.controlPoints[2],
    pointData.controlPoints[3],
    t);

    outputData.points[coord.x] = bezierPoint;
}
