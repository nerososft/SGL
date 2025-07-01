#version 450

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputStorageBuffer {
    float data[];
} inputBuffer;

layout (std430, binding = 1) buffer OutputStorageBuffer {
    float data[];
} outputBuffer;

layout (push_constant) uniform Params {
    uint ropeTheta;
    uint m;
// TODO: params
} params;

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= inputBuffer.data.length() / 2) {
        return;
    }

    uint index = coord.x;
    uint dim = inputBuffer.data.length();

    float theta = pow(params.ropeTheta, -2.0f * index / dim);
    float angle = params.m * theta;

    float real = inputBuffer.data[coord.x * 2];
    float imag = inputBuffer.data[coord.x * 2 + 1];
    vec2 rotated = mat2(cos(angle), -sin(angle), sin(angle), cos(angle)) * vec2(real, imag);
    outputBuffer.data[coord.x * 2] = rotated.x;
    outputBuffer.data[coord.x * 2 + 1] = rotated.y;
}
