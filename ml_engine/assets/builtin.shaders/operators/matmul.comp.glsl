#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputStorageBuffer {
    float data[];
} inputBuffer;

layout (std430, binding = 1) buffer InputStorageBuffer2 {
    float data[];
} inputBuffer2;

layout (std430, binding = 2) buffer OutputStorageBuffer {
    float data[];
} outputBuffer;

layout (push_constant) uniform Params {
    uint width1;// width1 == height2
    uint height1;// height1 = 输出矩阵行数
    uint width2;// width2 = 输出矩阵列数
    uint height2;
// TODO: params
} params;

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= params.height1 || coord.y >= params.width2) return;

    float sum = 0.0;
    for (uint k = 0; k < params.width1; ++k) {
        // 矩阵A行访问优化：A[coord.x][k]
        float a = inputBuffer.data[coord.x * params.width1 + k];
        // 矩阵B列访问优化：B[k][coord.y]
        float b = inputBuffer2.data[k * params.width2 + coord.y];
        sum += a * b;
    }

    // 写入输出矩阵C[coord.x][coord.y]
    outputBuffer.data[coord.x * params.width2 + coord.y] = sum;
}