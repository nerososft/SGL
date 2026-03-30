#version 450
layout (local_size_x = 1, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageStorageBuffer0 { uint pixels[]; } inputImage0;
layout (std430, binding = 1) buffer InputImageStorageBuffer1 { uint pixels[]; } inputImage1;
layout (std430, binding = 2) buffer InputImageStorageBuffer2 { uint pixels[]; } inputImage2;
layout (std430, binding = 3) buffer InputImageStorageBuffer3 { uint pixels[]; } inputImage3;
layout (std430, binding = 4) buffer InputImageStorageBuffer4 { uint pixels[]; } inputImage4;
layout (std430, binding = 5) buffer InputImageStorageBuffer5 { uint pixels[]; } inputImage5;
layout (std430, binding = 6) buffer InputImageStorageBuffer6 { uint pixels[]; } inputImage6;
layout (std430, binding = 7) buffer InputImageStorageBuffer7 { uint pixels[]; } inputImage7;
layout (std430, binding = 8) buffer InputImageStorageBuffer8 { uint pixels[]; } inputImage8;
layout (std430, binding = 9) buffer OutputImageStorageBuffer { uint pixels[]; } outputImage;

layout (push_constant) uniform FilterParams {
    uint imageTotalWidth;
    uint imageTotalHeight;
    uint channels;
    uint bytesPerLine;
    uint tileHeight;
    uint currentTileIndex;
    int radius;
} filterParams;

// ABGR
uint packColor(vec4 color) {
    return (
    uint(clamp(color.a, 0.0, 1.0) * 255.0) << 24) |
    (uint(clamp(color.b, 0.0, 1.0) * 255.0) << 16) |
    (uint(clamp(color.g, 0.0, 1.0) * 255.0) << 8) |
    (uint(clamp(color.r, 0.0, 1.0) * 255.0)
    );
}

// ABGR
vec4 unpackColor(uint color) {
    return vec4(
    float((color) & 0xFF) / 255.0f,
    float((color >> 8) & 0xFF) / 255.0f,
    float((color >> 16) & 0xFF) / 255.0f,
    float((color >> 24) & 0xFF) / 255.0f
    );
}

uint getPixel(uint localX, int localY) {
    int globalY = int(filterParams.currentTileIndex * filterParams.tileHeight) + localY;

    localX = min(localX, filterParams.imageTotalWidth - 1);
    globalY = clamp(globalY, 0, int(filterParams.imageTotalHeight) - 1);

    uint tileIdx = uint(globalY) / filterParams.tileHeight;
    uint tileLocalY = uint(globalY) % filterParams.tileHeight;
    uint pixelIdx = tileLocalY * filterParams.imageTotalWidth + localX;

    int inputBufferIdx = 4 + int(tileIdx) - int(filterParams.currentTileIndex);

    if (inputBufferIdx < 0) inputBufferIdx = 0;
    if (inputBufferIdx > 8) inputBufferIdx = 8;

    if (inputBufferIdx == 0) {
        return inputImage0.pixels[pixelIdx];
    } else if (inputBufferIdx == 1) {
        return inputImage1.pixels[pixelIdx];
    } else if (inputBufferIdx == 2) {
        return inputImage2.pixels[pixelIdx];
    } else if (inputBufferIdx == 3) {
        return inputImage3.pixels[pixelIdx];
    } else if (inputBufferIdx == 4) {
        return inputImage4.pixels[pixelIdx];
    } else if (inputBufferIdx == 5) {
        return inputImage5.pixels[pixelIdx];
    } else if (inputBufferIdx == 6) {
        return inputImage6.pixels[pixelIdx];
    } else if (inputBufferIdx == 7) {
        return inputImage7.pixels[pixelIdx];
    } else if (inputBufferIdx == 8) {
        return inputImage8.pixels[pixelIdx];
    }
}

void main() {
    uvec2 localCoord = gl_GlobalInvocationID.xy;
    uint localX = localCoord.x;
    uint localY = localCoord.y;

    if (localX >= filterParams.imageTotalWidth || localY >= filterParams.tileHeight) {
        return;
    }

    vec4 colorSum = vec4(0.0);
    float weightSum = 0.0;
    float sigma = float(filterParams.radius) / 3;
    for (int dy = -filterParams.radius; dy <= filterParams.radius; ++dy) {
        int sampleY = int(localY) + dy;
        float weight = exp(- float(dy * dy) / (2.0 * sigma * sigma));
        vec4 sampledColor = unpackColor(getPixel(localX, sampleY));

        colorSum += sampledColor * weight;
        weightSum += weight;
    }

    colorSum /= weightSum;

    uint idx = localY * filterParams.imageTotalWidth + localX;
    outputImage.pixels[idx] = packColor(colorSum);
}