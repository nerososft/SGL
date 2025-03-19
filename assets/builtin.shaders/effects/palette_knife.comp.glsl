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
    int quantScale;
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

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) {
        return;
    }

    const int radius = filterParams.radius;
    const float quantScale = filterParams.quantScale;
    const int maxBin = 16;
    const ivec3 quantFactor = ivec3(1, 4, 16);
    const uint imageSizeX = filterParams.width;
    const ivec2 imgBound = ivec2(filterParams.width, filterParams.height) - 1;

    vec4 histogram[maxBin];
    for (int i = 0; i < maxBin;) histogram[i++] = vec4(0);

    float maxCount = 0.0;
    int dominantBin = 0;
    ivec2 baseCoord = ivec2(coord);

    for (int dy = -radius; dy <= radius; dy += 2) {
        ivec2 yCoords = baseCoord.y + ivec2(dy, min(dy + 1, radius));
        bvec4 yValid = bvec4(yCoords[0] >= 0, yCoords[0] <= imgBound.y, yCoords[1] >= 0, yCoords[1] <= imgBound.y);

        for (int dx = -radius; dx <= radius; ++dx) {
            int xCoord = baseCoord.x + dx;
            bvec2 xValid = bvec2(xCoord >= 0, xCoord <= imgBound.x);

            vec3 colors[2];
            for (int i = 0; i < 2; ++i) {
                bool valid = (i == 0) ? (xValid[0] && yValid[0] && yValid[1]) : (xValid[1] && yValid[2] && yValid[3]);
                uint addr = valid ? (yCoords[i] * imageSizeX + xCoord) : 0;
                colors[i] = unpackColor(inputImage.pixels[addr]).rgb * vec3(valid);
            }

            for (int i = 0; i < 2; ++i) {
                ivec3 quant = ivec3(colors[i] * quantScale);
                int bin = (quant.r + (quant.g << 2) + (quant.b << 4)) & 15;

                vec4 entry = histogram[bin];
                entry.rgb += colors[i];
                entry.a += 1.0;
                if (entry.a > maxCount) { maxCount = entry.a; dominantBin = bin; }
                histogram[bin] = entry;
            }
        }
    }

    vec3 result = histogram[dominantBin].rgb / histogram[dominantBin].a;
    outputImage.pixels[coord.y * imageSizeX + coord.x] = packColor(vec4(result, 1.0));
}
