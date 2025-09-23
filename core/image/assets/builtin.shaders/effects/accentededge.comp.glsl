#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageStorageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer SobelxStorageBuffer {
    int value[];
} Sobelx;

layout (std430, binding = 2) buffer SobelyStorageBuffer {
    int value[];
} Sobely;

layout (std430, binding = 3) buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform FilterParams {
    uint width;
    uint height;
    uint channels;
    uint bytesPerLine;
    int type;
} Params;

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
    // 获取当前线程的全局ID
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;

    // 检查是否在图像范围内
    if (x >= Params.width || y >= Params.height) {
        return;
    }

    // 初始化梯度值
    int gRx = 0, gRy = 0;
    int gGx = 0, gGy = 0;
    int gBx = 0, gBy = 0;

    // 遍历Sobel算子的3x3邻域
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            // 获取邻域内的像素值，超出边界的像素使用0代替
            int pixelX = int(x) + i;
            int pixelY = int(y) + j;

            // 确保像素在图像范围内
            if (pixelX < 0 || pixelX >= int(Params.width) || pixelY < 0 || pixelY >= int(Params.height)) {
                 continue;
            }

            pixelX=clamp(pixelX,0,int(Params.width-1));
            pixelY=clamp(pixelY,0,int(Params.height-1));

            // 获取像素颜色
            uint pixel = inputImage.pixels[pixelY * Params.width + pixelX];
            vec4 color = unpackColor(pixel);

            // 获取Sobel算子的值
            int sobelX = Sobelx.value[(i + 1) * 3 + (j + 1)];
            int sobelY = Sobely.value[(i + 1) * 3 + (j + 1)];

            // 计算R通道梯度值
            gRx += sobelX * int(color.r * 255.0);
            gRy += sobelY * int(color.r * 255.0);

            // 计算G通道梯度值
            gGx += sobelX * int(color.g * 255.0);
            gGy += sobelY * int(color.g * 255.0);

            // 计算B通道梯度值
            gBx += sobelX * int(color.b * 255.0);
            gBy += sobelY * int(color.b * 255.0);
        }
    }

    // 计算梯度的幅值
    float gradientMagnitudeR = sqrt(float(gRx * gRx + gRy * gRy));
    float gradientMagnitudeG = sqrt(float(gGx * gGx + gGy * gGy));
    float gradientMagnitudeB = sqrt(float(gBx * gBx + gBy * gBy));

    // 对梯度值进行归一化处理，确保在[0, 255]范围内
    gradientMagnitudeR = clamp(gradientMagnitudeR, 0.0, 255.0);
    gradientMagnitudeG = clamp(gradientMagnitudeG, 0.0, 255.0);
    gradientMagnitudeB = clamp(gradientMagnitudeB, 0.0, 255.0);

    // 如果类型为0，则对梯度值进行反转
    if (Params.type == 0) {
        gradientMagnitudeR = 255.0 - gradientMagnitudeR;
        gradientMagnitudeG = 255.0 - gradientMagnitudeG;
        gradientMagnitudeB = 255.0 - gradientMagnitudeB;
    }

    // 设置输出图像的像素颜色
    vec4 gradientColor = vec4(
        gradientMagnitudeR / 255.0,
        gradientMagnitudeG / 255.0,
        gradientMagnitudeB / 255.0,
        1.0
    );

    outputImage.pixels[y * Params.width + x] = packColor(gradientColor);
}