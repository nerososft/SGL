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
    uint piece;
    uint pieceCount;
    float radius;
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
uvec4 unpackColor3u(uint color) {
    return uvec4((color) & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF, (color >> 24) & 0xFF);
}

void main() {
    ivec2 imgCoord = ivec2(gl_GlobalInvocationID.xy);
    if (any(greaterThanEqual(imgCoord, ivec2(filterParams.width / filterParams.pieceCount, filterParams.height)))) {
        return;
    }
    imgCoord.x += int((filterParams.width / filterParams.pieceCount) * filterParams.piece);

    // 计算有效半径
    int radius = int(ceil(filterParams.radius));
    int diameter = 2 * radius + 1;
    int totalPixels = diameter * diameter;

    // 初始化直方图
    uint rHist[256], gHist[256], bHist[256];
    for (int i = 0; i < 256; i++) {
        rHist[i] = 0;
        gHist[i] = 0;
        bHist[i] = 0;
    }

    // 收集邻域像素
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            ivec2 sampleCoord = imgCoord + ivec2(dx, dy);

            // 边界裁剪
            sampleCoord = clamp(sampleCoord, ivec2(0),
            ivec2(filterParams.width-1, filterParams.height-1));

            // 计算存储位置
            uint rowOffset = uint(sampleCoord.y) * (filterParams.bytesPerLine / 4);
            uint pixelIndex = rowOffset + uint(sampleCoord.x);

            // 解包颜色
            uvec4 colorInt = unpackColor3u(inputImage.pixels[pixelIndex]);

            rHist[colorInt.r]++;
            gHist[colorInt.g]++;
            bHist[colorInt.b]++;
        }
    }

    // 计算中值
    vec3 median = vec3(0.0);
    uint targetCount = (totalPixels + 1) / 2;

    // 红色通道
    uint countr = 0, countg = 0, countb = 0;
    int flagr = 1, flagg = 1, flagb = 1;

    #pragma unroll
    for (int i = 0; i < 256; i++) {
        countr += rHist[i];
        if ((countr >= targetCount) && (flagr!=0)) {
            median.r = float(i) / 255.0;
            flagr = 0;
        }
        countg += gHist[i];
        if ((countg >= targetCount) && (flagg!=0)) {
            median.g = float(i) / 255.0;
            flagg = 0;
        }
        countb += bHist[i];
        if ((countb >= targetCount) && (flagb!=0)) {
            median.b = float(i) / 255.0;
            flagb = 0;
        }
    }

    // 保持alpha通道不变
    vec4 origColor = unpackColor(inputImage.pixels[uint(imgCoord.y) * (filterParams.bytesPerLine / 4) + uint(imgCoord.x)]);

    // 打包输出
    uint outputIndex = uint(imgCoord.y) * (filterParams.bytesPerLine / 4) + uint(imgCoord.x);
    outputImage.pixels[outputIndex] = packColor(vec4(median, origColor.a));
}

