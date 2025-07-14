#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputStorageBuffer {
    float data[];
} inputBuffers[16];

layout (std430, binding = 16) buffer OutputStorageBuffer {
    float data[];
} outputBuffer;

layout (push_constant) uniform Params {
    uint nums;
    uint blockWidth;
    uint blockHeight;
    uint width;
    uint height;
} params;

void main() {
    uvec2 globalCoord = gl_GlobalInvocationID.xy;

    if (globalCoord.x >= params.width || globalCoord.y >= params.height) {
        return;
    }

    uint bufferIndex = globalCoord.x / params.blockWidth;
    if (bufferIndex >= params.nums) {
        return;
    }

    uint localX = globalCoord.x % params.blockWidth;
    uint inputIndex = globalCoord.y * params.blockWidth + localX;
    uint outputIndex = globalCoord.y * params.width + globalCoord.x;
    outputBuffer.data[outputIndex] = inputBuffers[bufferIndex].data[inputIndex];
}