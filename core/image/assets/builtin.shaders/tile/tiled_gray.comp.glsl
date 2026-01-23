#version 450
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageStorageBuffer0 { uint pixels[]; } inputImage[9];
layout (std430, binding = 9) buffer OutputImageStorageBuffer { uint pixels[]; } outputImage;

layout (push_constant) uniform FilterParams {
    uint imageTotalWidth;
    uint imageTotalHeight;
    uint channels;
    uint bytesPerLine;
    uint tileHeight;
    uint currentTileIndex;
    float redFactor;
    float greenFactor;
    float blueFactor;
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

    return inputImage[inputBufferIdx].pixels[pixelIdx];
}

void main() {
    uvec2 localCoord = gl_GlobalInvocationID.xy;
    uint localX = localCoord.x;
    uint localY = localCoord.y;

    if (localX >= filterParams.imageTotalWidth || localY >= filterParams.tileHeight) {
        return;
    }

    uint idx = localY * filterParams.imageTotalWidth + localX;

    uint pixel = getPixel(localX, int(localY));

    vec4 srcColor = unpackColor(pixel);
    float gray = dot(srcColor.rgb, vec3(filterParams.redFactor, filterParams.greenFactor, filterParams.blueFactor));

    outputImage.pixels[idx] = packColor(vec4(gray, gray, gray, srcColor.a));
}