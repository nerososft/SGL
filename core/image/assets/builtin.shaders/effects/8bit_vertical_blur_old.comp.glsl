#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageStorageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform FilterParams {
    uint width;
    uint height;
    uint channels;
    uint bytesPerLine;
    int radius;
} filterParams;

float unpack8BitSingleChannel(uint packedValue, uint byteOffset) {
    uint eightBitValue = (packedValue >> (byteOffset * 8)) & 0xFF;
    return float(eightBitValue) / 255.0;
}

uint pack8BitSingleChannel(uint originalPacked, float normalizedValue, uint byteOffset) {
    uint eightBitValue = uint(clamp(normalizedValue, 0.0, 1.0) * 255.0);
    originalPacked &= ~(0xFF << (byteOffset * 8));
    originalPacked |= (eightBitValue << (byteOffset * 8));
    return originalPacked;
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) {
        return;
    }

    uint globalPixelIndex = coord.x + coord.y * filterParams.width;
    float sigma = float(filterParams.radius) / 3;

    float colorSum = 0.0;
    float weightSum = 0.0;
    for (int dy = -filterParams.radius; dy <= filterParams.radius; ++dy) {
        int sampleY = int(coord.y) + dy;
        sampleY = clamp(sampleY, 0, int(filterParams.height) - 1);

        uint sampleGlobalIndex = coord.x + uint(sampleY) * filterParams.width;
        uint uintIndex = sampleGlobalIndex / 4;
        uint byteOffset = sampleGlobalIndex % 4;

        uint packedValue = inputImage.pixels[uintIndex];
        float sampledColor = unpack8BitSingleChannel(packedValue, byteOffset);

        float weight = exp(- float(dy * dy) / (2.0 * sigma * sigma));
        colorSum += sampledColor * weight;
        weightSum += weight;
    }

    float finalColor = colorSum / weightSum;
    uint targetUintIndex = globalPixelIndex / 4;
    uint targetByteOffset = globalPixelIndex % 4;
    uint originalPacked = outputImage.pixels[targetUintIndex];
    outputImage.pixels[targetUintIndex] = pack8BitSingleChannel(originalPacked, finalColor, targetByteOffset);
}