#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputStorageBuffer {
    float data[];
} inputBuffer;

layout (std430, binding = 1) buffer OutputStorageBuffer {
    float data[];
} outputBuffer;

layout (push_constant) uniform Params {
    uint width;
    uint height;
} params;

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;

    if (coord.x >= params.height || coord.y >= params.width) { return; }

    uint inputIndex = coord.x * params.width + coord.y;
    uint outputIndex = coord.y * params.height + coord.x;

    outputBuffer.data[outputIndex] = inputBuffer.data[inputIndex];
}