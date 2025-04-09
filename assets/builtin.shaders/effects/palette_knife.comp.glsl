#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageStorageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer QImageStorageBuffer {
    uint pixels[];
} Q;

layout (std430, binding = 2) buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform FilterParams {
    uint width;
    uint height;
    uint channels;
    uint bytesPerLine;
    int radius;
    int s1;
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

    uint index = coord.y * (filterParams.bytesPerLine / 4) + coord.x;
    vec4 currentColor = vec4(0.0f);
    uint currentMax = 0;

    uint histSize = 1000;
    uint hist[1000];
    for (uint i = 0; i < histSize; ++i) {
        hist[i] = 0;
    }

    // 扫描邻域
    for (int m = -filterParams.radius; m <= filterParams.radius; ++m) {
        int row = int(coord.y) + m;
        if (row < 0 || row >= filterParams.height) continue;

        for (int n = -filterParams.radius; n <= filterParams.radius; ++n) {
            int col = int(coord.x) + n;
            if (col < 0 || col >= filterParams.width) continue;

            uint neighborIndex = row * (filterParams.bytesPerLine / 4) + col;
            uint qVal = Q.pixels[neighborIndex];
            if (qVal >= histSize){
                qVal =  histSize - 1;
            }
            hist[qVal]++;
            if (hist[qVal] >= currentMax) {
                currentColor = unpackColor(inputImage.pixels[neighborIndex]);
                currentMax = hist[qVal];
            }
        }
    }

    // 设置输出颜
    outputImage.pixels[index] = packColor(currentColor);
}