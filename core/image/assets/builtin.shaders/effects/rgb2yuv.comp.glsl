#version 450

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageStorageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform FilterParams {
    uint inputWidth;       // 输入图像宽度（像素）
    uint inputHeight;      // 输入图像高度（像素）
    uint inputChannels;    // 输入图像通道数（应为4）
    uint inputBytesPerLine;// 输入图像每行字节数
    uint outputWidthStride;// Y平面每行字节数
    uint outputHeightStride;// Y平面高度（与输入高度相同）
    uint format;           // 保持为1表示NV12
} filterParams;

// 修复颜色通道提取顺序：正确从RGBA 32位值中提取R、G、B通道
vec4 unpackColor(uint color) {
    return vec4(
        float((color >> 24) & 0xFF) / 255.0f,  // R通道（最高8位）
        float((color >> 16) & 0xFF) / 255.0f,  // G通道
        float((color >> 8) & 0xFF) / 255.0f,   // B通道
        float(color & 0xFF) / 255.0f           // A通道（最低8位）
    );
}

void main() {
    uvec2 blockId = gl_GlobalInvocationID.xy;
    uint baseX = blockId.x * 2;  // 2x2块的起始X坐标
    uint baseY = blockId.y * 2;  // 2x2块的起始Y坐标

    // 存储块内像素数据
    vec3 blockRGB[2][2];
    float yValues[2][2];
    bool inBounds[2][2];
    vec3 rgbSum = vec3(0);
    int count = 0;

    // 初始化边界标记
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            inBounds[i][j] = false;
        }
    }

    // 读取2x2块内的像素并计算Y值
    for (uint i = 0; i < 2; i++) {
        for (uint j = 0; j < 2; j++) {
            uint x = baseX + i;
            uint y = baseY + j;

            if (x < filterParams.inputWidth && y < filterParams.inputHeight) {
                uint inputIndex = y * filterParams.inputWidth + x;
                vec4 rgba = unpackColor(inputImage.pixels[inputIndex]);
                blockRGB[i][j] = rgba.rgb;
                inBounds[i][j] = true;

                // 累积RGB值用于计算平均UV
                rgbSum += blockRGB[i][j];
                count++;

                // 计算Y分量（BT.601标准）
                float y = 0.299 * rgba.r + 0.587 * rgba.g + 0.114 * rgba.b;
                y = clamp(y, 0.0, 1.0);
                yValues[i][j] = y;
            }
        }
    }

    // 计算UV分量（基于块内像素的平均值）
    vec2 uv = vec2(0.5, 0.5);  // 默认灰色
    if (count > 0) {
        vec3 avgRGB = rgbSum / float(count);
        // 计算U分量（范围[-0.436, 0.436] -> [0, 1]）
        float u = -0.14713 * avgRGB.r - 0.28886 * avgRGB.g + 0.436 * avgRGB.b + 0.5;
        // 计算V分量（范围[-0.615, 0.615] -> [0, 1]）
        float v = 0.615 * avgRGB.r - 0.51499 * avgRGB.g - 0.10001 * avgRGB.b + 0.5;
        uv = clamp(vec2(u, v), 0.0, 1.0);
    }

    // 转换为字节值
    uint uByte = min(uint(uv.x * 255.0), 255u);
    uint vByte = min(uint(uv.y * 255.0), 255u);

    // ================================================
    // 写入Y平面（每个像素1字节，4个像素打包为1个uint）
    // ================================================
    for (uint j = 0; j < 2; j++) {
        for (uint i = 0; i < 2; i++) {
            uint x = baseX + i;
            uint y = baseY + j;

            if (inBounds[i][j]) {
                // 计算Y在输出缓冲区中的字节偏移
                uint yByteOffset = y * filterParams.outputWidthStride + x;
                // 转换为uint索引和字节位置
                uint yUintIndex = yByteOffset / 4;
                uint bytePos = yByteOffset % 4;  // 0-3（表示uint中的第几个字节）

                // 计算Y的字节值
                uint yValue = uint(yValues[i][j] * 255.0) & 0xFFu;

                // 写入到输出缓冲区（保留其他字节的值）
                uint mask = ~(0xFFu << (bytePos * 8));
                outputImage.pixels[yUintIndex] = (outputImage.pixels[yUintIndex] & mask) |
                                                (yValue << (bytePos * 8));
            }
        }
    }

    // ================================================
    // 写入UV平面（每个2x2块对应一个UV对，存储为2字节）
    // ================================================
    // 仅在块的左上角像素位置处理UV（避免重复处理）
    if (baseX < filterParams.inputWidth && baseY < filterParams.inputHeight) {
        // 计算UV平面的起始uint索引（Y平面总字节数/4，向上取整）
        uint yTotalBytes = filterParams.outputWidthStride * filterParams.outputHeightStride;
        uint uvBase = (yTotalBytes + 3) / 4;  // 转换为uint索引

        // 计算UV在平面中的位置（UV平面尺寸是Y平面的1/2 x 1/2）
        uint uvRow = blockId.y;
        uint uvCol = blockId.x;

        // 计算UV的字节偏移
        uint uvByteStride = filterParams.outputWidthStride / 2;  // UV平面每行字节数
        uint uvByteOffset = uvRow * uvByteStride + uvCol * 2;    // 每个UV对占2字节

        // 转换为uint索引和偏移
        uint uvUintIndex = uvBase + uvByteOffset / 4;
        uint uvShift = (uvByteOffset % 4) * 8;  // 0或16（表示低16位或高16位）

        // 打包UV值（U在低8位，V在高8位）
        uint uvValue = (uByte) | (vByte << 8);

        // 写入到输出缓冲区
        uint mask = ~(0xFFFFu << uvShift);
        outputImage.pixels[uvUintIndex] = (outputImage.pixels[uvUintIndex] & mask) |
                                         (uvValue << uvShift);
    }
}
