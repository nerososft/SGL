#version 450

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputStorageBuffer {
    float data[];
} inputBuffer;

layout (std430, binding = 1) buffer OutputStorageBuffer {
    float data[];
} outputBuffer;

layout (push_constant) uniform Params {
    uint width;
    uint height;
// TODO: params
} params;

const float PI = 3.1415926f;

float sigmoid(float x) {
    return 1.0f / (1.0f + exp(-x));
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= inputBuffer.data.length()) {
        return;
    }
    float x = inputBuffer.data[coord.x];
    outputBuffer.data[coord.x] = x * sigmoid(x);
}
