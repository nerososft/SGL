#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) readonly buffer InputImageStorageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) writeonly buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform FilterParams {
    uint width;
    uint height;
    uint channels;
    uint bytesPerLine;
    float scale;
    float sigmaScale;
} params;

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
    // 计算输出图像的宽高（基于缩放比例）
    uint outWidth = uint(params.width * params.scale);
    uint outHeight = uint(params.height * params.scale);

    // 检查当前坐标是否在输出图像范围内
    if (coord.x >= outWidth || coord.y >= outHeight) {
        return;
    }

    // 将输出坐标映射回输入图像坐标（浮点数）
    float inX = float(coord.x) / params.scale;
    float inY = float(coord.y) / params.scale;

    // 计算高斯核半径（通常为3倍sigma，这里基于sigmaScale计算）
    float sigma = params.sigmaScale;
    int kernelRadius = int(ceil(3.0 * sigma));
    int kernelSize = 2 * kernelRadius + 1;

    vec4 finalColor = vec4(0.0);
    float weightSum = 0.0;

    // 遍历高斯核范围内的像素
    for (int dy = -kernelRadius; dy <= kernelRadius; dy++) {
        for (int dx = -kernelRadius; dx <= kernelRadius; dx++) {
            // 计算输入图像中的采样坐标
            float sampleX = inX + float(dx);
            float sampleY = inY + float(dy);

            // 确保采样坐标在输入图像范围内（边界处理）
            if (sampleX < 0.0 || sampleX >= float(params.width) ||
            sampleY < 0.0 || sampleY >= float(params.height)) {
                continue;
            }

            // 计算高斯权重 (G(x,y) = (1/(2πσ²)) * e^(-(x²+y²)/(2σ²)))
            float xDist = float(dx);
            float yDist = float(dy);
            float exponent = -(xDist * xDist + yDist * yDist) / (2.0 * sigma * sigma);
            float weight = exp(exponent);

            // 计算输入图像的索引并获取颜色
            uint inCoordX = uint(sampleX);
            uint inCoordY = uint(sampleY);
            uint inIndex = inCoordY * params.width + inCoordX;
            vec4 color = unpackColor(inputImage.pixels[inIndex]);

            // 累加加权颜色
            finalColor += color * weight;
            weightSum += weight;
        }
    }

    // 归一化颜色（除以权重总和）
    if (weightSum > 0.0) {
        finalColor /= weightSum;
    }

    // 计算输出图像的索引并写入结果
    uint outputIndex = coord.y * outWidth + coord.x;
    outputImage.pixels[outputIndex] = packColor(clamp(finalColor, 0.0, 1.0));
}