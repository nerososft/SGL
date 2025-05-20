#version 450

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer PointData {
    vec2 points[];
} pointData;

layout (std430, binding = 1) buffer OutputData {
    vec2 points[];
} outputData;

layout (push_constant) uniform Params {
    mat4 transform;
} params;

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x > pointData.points.length()) {
        return;
    }

    vec4 transformed = params.transform * vec4(pointData.points[coord.x], 1, 1);

    outputData.points[coord.x] = vec2(transformed.x, transformed.y);
}
