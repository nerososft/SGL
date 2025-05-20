#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

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

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
}
