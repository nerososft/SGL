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
    mat4 transform;
} filterParams;

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) {
        return;
    }

    // 使用齐次坐标进行变换（第四个分量设为1.0）
    vec4 transformed = filterParams.transform * vec4(float(coord.x), float(coord.y), 1.0, 1.0);

    // 执行透视除法
    vec2 transformedCoord = transformed.xy / transformed.w;

    // 检查变换后的坐标是否在有效范围内（包括负值检查）
    if (transformedCoord.x < 0 || transformedCoord.y < 0 ||
    transformedCoord.x >= filterParams.width || transformedCoord.y >= filterParams.height) {
        return;
    }

    // 计算源像素索引
    uint index = coord.y * (filterParams.bytesPerLine / 4) + coord.x;
    uint pixel = inputImage.pixels[index];

    // 计算目标像素索引，使用四舍五入而不是截断
    uint transformedX = uint(round(transformedCoord.x));
    uint transformedY = uint(round(transformedCoord.y));
    uint transformedIndex = transformedY * (filterParams.bytesPerLine / 4) + transformedX;

    // 确保不会写入超出边界
    if (transformedIndex < filterParams.width * filterParams.height) {
        outputImage.pixels[transformedIndex] = pixel;
    }
}