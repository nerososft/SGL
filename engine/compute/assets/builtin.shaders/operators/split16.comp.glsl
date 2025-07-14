#version 450

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputStorageBuffer {
    float data[];
} inputBuffer;

layout (std430, binding = 1) buffer OutputStorageBuffer {
    float data[];
} outputBuffers[16];

layout (push_constant) uniform Params {
    uint dim;
    uint nums;
} params;

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= params.dim) {
        return;
    }
    for (uint buffer_idx = 0; buffer_idx < 16; buffer_idx++) {
        outputBuffers[buffer_idx].data[coord.x] = inputBuffer.data[buffer_idx * params.dim + coord.x];
    }
}
