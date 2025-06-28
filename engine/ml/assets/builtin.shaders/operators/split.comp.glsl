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
layout (std430, binding = 9) buffer OutputStorageBuffer9 {
    float data[];
} outputBuffer9;
layout (std430, binding = 10) buffer OutputStorageBuffer10 {
    float data[];
} outputBuffer10;
layout (std430, binding = 11) buffer OutputStorageBuffer11 {
    float data[];
} outputBuffer11;
layout (std430, binding = 12) buffer OutputStorageBuffer12 {
    float data[];
} outputBuffer12;
layout (std430, binding = 13) buffer OutputStorageBuffer13 {
    float data[];
} outputBuffer13;
layout (std430, binding = 14) buffer OutputStorageBuffer14 {
    float data[];
} outputBuffer14;
layout (std430, binding = 15) buffer OutputStorageBuffer15 {
    float data[];
} outputBuffer15;
layout (std430, binding = 16) buffer OutputStorageBuffer16 {
    float data[];
} outputBuffer16;

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
    outputBuffer9.data[coord.x] = inputBuffer.data[8 * params.dim + coord.x];
    outputBuffer10.data[coord.x] = inputBuffer.data[9 * params.dim + coord.x];
    outputBuffer11.data[coord.x] = inputBuffer.data[10 * params.dim + coord.x];
    outputBuffer12.data[coord.x] = inputBuffer.data[11 * params.dim + coord.x];
    outputBuffer13.data[coord.x] = inputBuffer.data[12 * params.dim + coord.x];
    outputBuffer14.data[coord.x] = inputBuffer.data[13 * params.dim + coord.x];
    outputBuffer15.data[coord.x] = inputBuffer.data[14 * params.dim + coord.x];
    outputBuffer16.data[coord.x] = inputBuffer.data[15 * params.dim + coord.x];
}
