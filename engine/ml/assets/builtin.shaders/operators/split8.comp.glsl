#version 450

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputStorageBuffer {
    float data[];
} inputBuffer;

layout (std430, binding = 1) buffer OutputStorageBuffer1 {
    float data[];
} outputBuffer1;
layout (std430, binding = 2) buffer OutputStorageBuffer2 {
    float data[];
} outputBuffer2;
layout (std430, binding = 3) buffer OutputStorageBuffer3 {
    float data[];
} outputBuffer3;
layout (std430, binding = 4) buffer OutputStorageBuffer4 {
    float data[];
} outputBuffer4;
layout (std430, binding = 5) buffer OutputStorageBuffer5 {
    float data[];
} outputBuffer5;
layout (std430, binding = 6) buffer OutputStorageBuffer6 {
    float data[];
} outputBuffer6;
layout (std430, binding = 7) buffer OutputStorageBuffer7 {
    float data[];
} outputBuffer7;
layout (std430, binding = 8) buffer OutputStorageBuffer8 {
    float data[];
} outputBuffer8;

layout (push_constant) uniform Params {
    uint dim;
    uint nums;
} params;

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= params.dim) {
        return;
    }
    outputBuffer1.data[coord.x] = inputBuffer.data[0 * params.dim + coord.x];
    outputBuffer2.data[coord.x] = inputBuffer.data[1 * params.dim + coord.x];
    outputBuffer3.data[coord.x] = inputBuffer.data[2 * params.dim + coord.x];
    outputBuffer4.data[coord.x] = inputBuffer.data[3 * params.dim + coord.x];
    outputBuffer5.data[coord.x] = inputBuffer.data[4 * params.dim + coord.x];
    outputBuffer6.data[coord.x] = inputBuffer.data[5 * params.dim + coord.x];
    outputBuffer7.data[coord.x] = inputBuffer.data[6 * params.dim + coord.x];
    outputBuffer8.data[coord.x] = inputBuffer.data[7 * params.dim + coord.x];
}
