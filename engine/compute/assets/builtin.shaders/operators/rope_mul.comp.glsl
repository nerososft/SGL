#version 450

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputStorageBuffer1 {
    float data[];
} inputBuffer1;

layout (std430, binding = 1) buffer InputStorageBuffer2 {
    float data[];
} inputBuffer2;

layout (std430, binding = 2) buffer OutputStorageBuffer {
    float data[];
} outputBuffer;

layout (push_constant) uniform Params {
    uint ropeTheta;
    uint m;
    uint n;
} params;

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= inputBuffer1.data.length() / 2) {
        return;
    }
    if (inputBuffer1.data.length() != inputBuffer2.data.length()) {
        return;
    }

    uint index = coord.x;
    uint dim = inputBuffer1.data.length();

    float theta = pow(params.ropeTheta, -2.0f * index / dim);

    float angle1 = params.m * theta;
    float real1 = inputBuffer1.data[coord.x * 2];
    float imag1 = inputBuffer1.data[coord.x * 2 + 1];
    vec2 rotated1 = mat2(cos(angle1), -sin(angle1), sin(angle1), cos(angle1)) * vec2(real1, imag1);


    float angle2 = params.n * theta;
    float real2 = inputBuffer2.data[coord.x * 2];
    float imag2 = inputBuffer2.data[coord.x * 2 + 1];
    vec2 rotated2 = mat2(cos(angle2), -sin(angle2), sin(angle2), cos(angle2)) * vec2(real2, imag2);
    outputBuffer.data[coord.x * 2] = rotated1.x * rotated2.x;
    outputBuffer.data[coord.x * 2 + 1] = rotated1.y * rotated2.y;
}
