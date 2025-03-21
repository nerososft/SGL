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
    uint gridSize;
    uint enableEdge;
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
float hash(uvec2 coord) {
    return fract(sin(float(coord.x * 12.9898 + coord.y * 78.233)) * 43758.5453);
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) return;

    vec4 finalColor = vec4(0);
    float minDist = 1e9;
    uvec2 nearestPoint = uvec2(0);
    float cellSize = float(filterParams.gridSize);
    const int RANGE = 1;

    for (int dy = -RANGE; dy <= RANGE; ++dy) {
        for (int dx = -RANGE; dx <= RANGE; ++dx) {
            uvec2 grid = (coord / filterParams.gridSize) + uvec2(dx, dy);
            float xOffset = (grid.y % 2 == 1) ? cellSize * 0.5 : -cellSize * 0.5;

            // 六边形随机点生成
            float angle = hash(grid) * 6.2832;
            float radius = sqrt(hash(grid + 1u)) * cellSize * 0.433;
            vec2 randOffset = vec2(cos(angle), sin(angle)) * radius;
            vec2 featurePos = vec2(grid) * cellSize + vec2(xOffset, 0) + randOffset;

            featurePos = clamp(featurePos, vec2(0), vec2(filterParams.width - 1, filterParams.height - 1));

            // 六边形轴向距离计算
            vec2 delta = (featurePos - vec2(coord)) / cellSize;
            float q = delta.x;
            float r = delta.y - delta.x * 0.5;
            float dist = (abs(q) + abs(r) + abs(-q - r)) / 2.0;

            if (dist < minDist) {
                minDist = dist;
                nearestPoint = uvec2(featurePos);
            }
        }
    }

    uint color = inputImage.pixels[nearestPoint.y * filterParams.bytesPerLine / 4 + nearestPoint.x];
    finalColor = unpackColor(color);

    if (filterParams.enableEdge > 0) {
        float edge = smoothstep(0.0, 0.05, minDist); // 调整后的边缘阈值
        finalColor.rgb *= edge;
    }

    outputImage.pixels[coord.y * filterParams.bytesPerLine / 4 + coord.x] = packColor(finalColor);
}