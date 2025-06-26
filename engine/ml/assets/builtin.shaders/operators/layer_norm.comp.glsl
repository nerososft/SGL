#version 450

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputStorageBuffer {
    float data[];
} inputBuffer;

layout (std430, binding = 1) buffer WeightStorageBuffer {
    float data[];
} weightBuffer;

layout (std430, binding = 2) buffer BiasStorageBuffer {
    float data[];
} biasBuffer;

layout (std430, binding = 3) buffer OutputStorageBuffer {
    float data[];
} outputBuffer;

layout (push_constant) uniform Params {
    float avg;
    float variance;
    float epsilon;
    bool weightEnable;
    bool biasEnable;
} params;

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= inputBuffer.data.length()) {
        return;
    }

    float weight =  params.weightEnable ? weightBuffer.data[coord.x] : 1.0f;
    float bias = params.biasEnable ? biasBuffer.data[coord.x] : 0.0f;

    outputBuffer.data[coord.x] = weight * ((inputBuffer.data[coord.x] - params.avg) / sqrt(params.variance + params.epsilon)) + bias;
}
