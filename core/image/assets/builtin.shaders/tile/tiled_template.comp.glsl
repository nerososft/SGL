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
    // TODO: other params for filter
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

uint getPixel(uint globalX, uint globalY) {
    uint currentTileYStart = filterParams.tileHeight * filterParams.currentTileIndex;
    uint currentTileYEnd = filterParams.tileHeight * (filterParams.currentTileIndex + 1);

    if (globalY >= currentTileYStart && globalY < currentTileYEnd) {
        uint localX = globalX;
        uint localY = globalY - currentTileYStart;

        uint localIdx = localY * filterParams.imageTotalWidth + localX;
        return inputImage[4].pixels[localIdx];
    }
    if (globalY < currentTileYStart) {
        int yOffset = int(globalY) - int(currentTileYStart);
        int upTileCount = (-yOffset + int(filterParams.tileHeight) - 1) / int(filterParams.tileHeight);
        int inputTileIdx = 4 - upTileCount;

        inputTileIdx = max(inputTileIdx, 0);
        uint localYInUpTile = uint(int(filterParams.tileHeight) + yOffset % int(filterParams.tileHeight));
        localYInUpTile = min(localYInUpTile, filterParams.tileHeight - 1);

        uint pixelIdx = localYInUpTile * filterParams.imageTotalWidth + globalX;
        return inputImage[inputTileIdx].pixels[pixelIdx];
    }
    if (globalY >= currentTileYEnd) {
        int yOffset = int(globalY) - int(currentTileYEnd);
        int downTileCount = (yOffset + int(filterParams.tileHeight) - 1) / int(filterParams.tileHeight);
        int inputTileIdx = 5 + downTileCount;
        inputTileIdx = min(inputTileIdx, 8);

        uint localYInDownTile = uint(yOffset % int(filterParams.tileHeight));
        localYInDownTile = min(localYInDownTile, filterParams.tileHeight - 1);

        uint pixelIdx = localYInDownTile * filterParams.imageTotalWidth + globalX;
        return inputImage[inputTileIdx].pixels[pixelIdx];
    }
    return 0;
}

void main() {
    uvec2 localCoord = gl_GlobalInvocationID.xy;
    uint localX = localCoord.x;
    uint localY = localCoord.y;

    if (localX >= filterParams.imageTotalWidth || localY >= filterParams.tileHeight) {
        return;
    }

    uint globalX = localX;
    uint globalY = filterParams.currentTileIndex * filterParams.tileHeight + localY;
    if (globalY >= filterParams.imageTotalHeight) {
        return;
    }

    int currentTileIdxInt = int(filterParams.currentTileIndex);
    int minReginYInt = (currentTileIdxInt - 4) * int(filterParams.tileHeight);
    int maxReginYInt = (currentTileIdxInt + 5) * int(filterParams.tileHeight);
    minReginYInt = max(minReginYInt, 0);
    maxReginYInt = min(maxReginYInt, int(filterParams.imageTotalHeight));
    uint minReginY = uint(minReginYInt);
    uint maxReginY = uint(maxReginYInt);
    if (globalY < minReginY || globalY >= maxReginY) {
        return;
    }

    uint pixel = getPixel(globalX, globalY);
    vec4 srcColor = unpackColor(pixel);

    // TODO: impl this

    uint idx = localY * filterParams.imageTotalWidth + localX;
    outputImage.pixels[idx] = packColor(vec4(0, 0, 0, srcColor.a));
}