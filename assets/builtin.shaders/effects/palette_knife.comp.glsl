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
    vec4 color ;//= unpackColor(inputImage.pixels[index]);

    int histSize = 1000;
    int hist[1000];
    vec3 C[1000];
    for (int i = 0; i < histSize; ++i) {
       hist[i] = 0;
       C[i] = vec3(0.0);
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
            hist[qVal]++;
            C[qVal] = unpackColor(inputImage.pixels[neighborIndex]).rgb;
        }
    }

    // 找到直方图最大值
    int maxVal = 0;
    int maxIdx = 0;
    for (int i = 1; i < histSize; ++i) {
        if (hist[i] > maxVal) {
            maxVal = hist[i];
            maxIdx = i;
        }
    }

    // 设置输出颜色
    color.rgb = C[maxIdx];
    outputImage.pixels[index] = packColor(color);
}