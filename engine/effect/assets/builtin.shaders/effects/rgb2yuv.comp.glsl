#version 450

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageStorageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform FilterParams {
    uint inputWidth;
    uint inputHeight;
    uint inputChannels;
    uint inputBytesPerLine;
    uint outputWidthStride; // Stride in uints for Y plane
    uint outputHeightStride; // Height for Y plane (same as inputHeight)
    uint format; // Keep as 1 for NV12
} filterParams;

vec4 unpackColor(uint color) {
    return vec4(
    float(color & 0xFF) / 255.0f,
    float((color >> 8) & 0xFF) / 255.0f,
    float((color >> 16) & 0xFF) / 255.0f,
    float((color >> 24) & 0xFF) / 255.0f
    );
}

uint pack4Bytes(uint b0, uint b1, uint b2, uint b3) {
    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

void main() {
    uvec2 blockId = gl_GlobalInvocationID.xy;
    uint baseX = blockId.x * 2;
    uint baseY = blockId.y * 2;

    // Process a 2x2 block
    vec3 blockRGB[2][2];
    float yValues[2][2];
    bool inBounds[2][2];
    vec3 rgbSum = vec3(0);
    int count = 0;

    // Initialize all to false
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            inBounds[i][j] = false;
        }
    }

    // Read and process all 4 pixels in the block
    for (uint i = 0; i < 2; i++) {
        for (uint j = 0; j < 2; j++) {
            uint x = baseX + i;
            uint y = baseY + j;

            if (x < filterParams.inputWidth && y < filterParams.inputHeight) {
                uint inputIndex = y * filterParams.inputWidth + x;
                vec4 rgba = unpackColor(inputImage.pixels[inputIndex]);
                blockRGB[i][j] = rgba.rgb;
                inBounds[i][j] = true;

                // Accumulate for UV calculation
                rgbSum += blockRGB[i][j];
                count++;

                // Calculate Y for all pixels
                vec3 rgb = blockRGB[i][j];
                float y = 0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b;
                y = clamp(y, 0.0, 1.0);
                yValues[i][j] = y;
            }
        }
    }

    // Calculate UV only if there are valid pixels
    vec2 uv = vec2(0.5, 0.5);  // Default gray if no pixels
    if (count > 0) {
        vec3 avgRGB = rgbSum / float(count);
        float u = -0.14713 * avgRGB.r - 0.28886 * avgRGB.g + 0.436 * avgRGB.b + 0.5;
        float v = 0.615 * avgRGB.r - 0.51499 * avgRGB.g - 0.10001 * avgRGB.b + 0.5;
        uv = clamp(vec2(u, v), 0.0, 1.0);
    }

    // Convert to byte values
    uint uByte = min(uint(uv.x * 255.0), 255u);
    uint vByte = min(uint(uv.y * 255.0), 255u);

    // ================================================
    // Write Y Plane (packing 4 Y bytes into each uint)
    // ================================================
    for (uint j = 0; j < 2; j++) {
        uint row = baseY + j;
        if (row < filterParams.inputHeight) {
            uint yIndex = row * filterParams.outputWidthStride + blockId.x;
            uint packedY = outputImage.pixels[yIndex];  // Read existing data

            // Pack 2 pixels at a time for this row (2 bytes each)
            for (uint i = 0; i < 2; i += 2) {
                uint b0 = (baseX + i < filterParams.inputWidth && inBounds[i][j]) ?
                uint(yValues[i][j] * 255.0) & 0xFF : 0u;
                uint b1 = (baseX + i + 1 < filterParams.inputWidth && inBounds[i+1][j]) ?
                uint(yValues[i+1][j] * 255.0) & 0xFF : 0u;

                // Clear positions before writing
                uint shift = i * 8;  // 0 or 16
                uint mask = ~(0xFF | (0xFF << 8)) << shift;
                packedY &= mask;
                packedY |= (b0 | (b1 << 8)) << shift;
            }

            outputImage.pixels[yIndex] = packedY;
        }
    }

    // ================================================
    // Write UV Plane (only for top-left pixel of block)
    // ================================================
    if (baseY < filterParams.inputHeight && baseX < filterParams.inputWidth) {
        uint uvBase = filterParams.outputWidthStride * filterParams.outputHeightStride;
        uint uvRow = blockId.y;
        uint uvCol = blockId.x;
        uint uvIndex = uvBase + uvRow * filterParams.outputWidthStride + uvCol;

        uint packedUV = outputImage.pixels[uvIndex];  // Read existing data
        uint byteOffset = (blockId.x % 2) * 16;       // Low or high 16 bits
        uint mask = 0xFFFF << byteOffset;
        uint newUV = (uByte | (vByte << 8)) << byteOffset;

        packedUV = (packedUV & ~mask) | newUV;
        outputImage.pixels[uvIndex] = packedUV;
    }
}