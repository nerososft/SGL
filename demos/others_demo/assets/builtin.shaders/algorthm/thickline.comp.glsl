#version 450
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer PathBuffer {
    vec2 pathPoints[];
};

layout(std430, binding = 1) buffer ThickPathBuffer {
    vec2 thickPathPoints[];
};

layout (push_constant) uniform Params {
    float thickness;
    float endThickness;
    float dz;
};

void main() {
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= pathPoints.length()) return;

    vec2 point = pathPoints[idx];



    thickPathPoints[idx * 2] = point;
    thickPathPoints[idx * 2 + 1] = point;
}