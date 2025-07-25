#version 450

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputStorageBuffer {
    float data[];
} inputBuffer;

layout (std430, binding = 1) buffer OutputStorageBuffer {
    float data[];
} outputBuffer;

layout (push_constant) uniform Params {
    float sum;
    float max;
// TODO: params
} params;

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= inputBuffer.data.length()) {
        return;
    }
    float sum = max(params.sum, 1e-10);
    outputBuffer.data[coord.x] = exp(inputBuffer.data[coord.x] - params.max) / sum;
}
