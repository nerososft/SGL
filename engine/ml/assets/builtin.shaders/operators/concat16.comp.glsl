#version 450

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputStorageBuffer {
    float data[];
} inputBuffers[16];

layout (std430, binding = 16) buffer OutputStorageBuffer {
    float data[];
} outputBuffer;

layout (push_constant) uniform Params {
    uint dim;
    uint nums;
    uint dup;
} params;

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= params.dim) {
        return;
    }

    uint base_stride = params.dim * params.dup;
    for (uint buffer_idx = 0; buffer_idx < 16; buffer_idx++) {
        for (uint copy_idx = 0; copy_idx < params.dup; copy_idx++) {
            uint output_idx = buffer_idx * base_stride + copy_idx * params.dim + coord.x;
            outputBuffer.data[output_idx] = inputBuffers[buffer_idx].data[coord.x];
        }
    }
}
