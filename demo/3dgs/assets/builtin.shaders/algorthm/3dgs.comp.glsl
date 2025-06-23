#version 450
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct GaussianPoint3D {
    vec4 position;
    vec4 color;
    vec4 scale;
    vec4 rotate;
    vec4 opacity;
};

layout(set = 0, binding = 0) readonly buffer InputPoints {
    GaussianPoint3D points[];
} inputPoints;

layout(set = 0, binding = 1) buffer PixelMap {
    uint pixels[];
} pixelMap;

layout(set = 0, binding = 2) buffer DepthMap {
    float depth[];
} depthMap;

layout(push_constant) uniform Params {
    mat4 view;
    mat4 proj;
    uint numPoints;
    uint width;
    uint height;
} params;

uint packColor(vec4 color) {
    return (
    uint(clamp(color.a, 0.0, 1.0) * 255.0) << 24) |
    (uint(clamp(color.b, 0.0, 1.0) * 255.0) << 16) |
    (uint(clamp(color.g, 0.0, 1.0) * 255.0) << 8) |
    (uint(clamp(color.r, 0.0, 1.0) * 255.0)
    );
}

vec4 unpackColor(uint color) {
    return vec4(
    float((color) & 0xFF) / 255.0f,
    float((color >> 8) & 0xFF) / 255.0f,
    float((color >> 16) & 0xFF) / 255.0f,
    float((color >> 24) & 0xFF) / 255.0f
    );
}

// 计算旋转矩阵（使用四元数）
mat3 quaternionToMatrix(vec4 q) {
    // 归一化四元数以确保数值稳定性
    float len = length(q);
    if (len < 0.0001) return mat3(1.0);  // 单位矩阵
    q /= len;

    float qx = q.x, qy = q.y, qz = q.z, qw = q.w;
    float qx2 = qx * qx, qy2 = qy * qy, qz2 = qz * qz;

    return mat3(
    1.0 - 2.0 * (qy2 + qz2), 2.0 * (qx * qy - qw * qz), 2.0 * (qx * qz + qw * qy),
    2.0 * (qx * qy + qw * qz), 1.0 - 2.0 * (qx2 + qz2), 2.0 * (qy * qz - qw * qx),
    2.0 * (qx * qz - qw * qy), 2.0 * (qy * qz + qw * qx), 1.0 - 2.0 * (qx2 + qy2)
    );
}

void main() {
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= params.numPoints) return;

    GaussianPoint3D point = inputPoints.points[idx];
    vec3 position = point.position.xyz;
    vec4 color = point.color;
    vec3 scale = point.scale.xyz;
    vec4 rotation = point.rotate;
    float opacity = point.opacity.x;

    if (any(isnan(position)) || any(isinf(position)) ||
    any(isnan(scale)) || any(isinf(scale)) ||
    any(isnan(rotation)) || any(isinf(rotation)) ||
    isnan(opacity) || isinf(opacity)) {
        return;
    }

    // 应用视图和投影变换
    vec4 viewPos = params.view * vec4(position, 1.0);

    // 检查点是否在相机前方
    if (viewPos.z >= 0.0) return;  // 在相机后方，不可见

    vec4 clipPos = params.proj * viewPos;

    // 透视除法得到NDC坐标
    vec3 ndcPos = clipPos.xyz / clipPos.w;

    // 检查点是否在视锥体内
    if (abs(ndcPos.x) > 1.0 || abs(ndcPos.y) > 1.0 || abs(ndcPos.z) > 1.0)
    return;

    // 转换到屏幕空间坐标
    vec2 screenPos = vec2(
    (ndcPos.x + 1.0) * 0.5 * float(params.width),
    (1.0 - (ndcPos.y + 1.0) * 0.5) * float(params.height)
    );

    // 创建旋转矩阵
    mat3 rotationMatrix = quaternionToMatrix(rotation);

    // 计算点在屏幕上的影响半径（增加缩放系数）
    float baseRadius = max(scale.x, max(scale.y, scale.z)) * 30.0 / (-viewPos.z);

    // 确保半径在有效范围内
    baseRadius = clamp(baseRadius, 2.0, 400.0);  // 增大最小半径

    // 计算变换后的尺度（用于各向异性高斯）
    vec3 transformedScale = rotationMatrix * scale;

    // 计算x和y方向的实际半径（调整比例计算）
    float radiusX = baseRadius * (scale.x / max(scale.x, max(scale.y, scale.z)));
    float radiusY = baseRadius * (scale.y / max(scale.x, max(scale.y, scale.z)));

    // 计算旋转后的方向向量（用于椭圆形状）
    vec2 dirX = normalize(vec2(rotationMatrix[0][0], rotationMatrix[1][0]));
    vec2 dirY = normalize(vec2(rotationMatrix[0][1], rotationMatrix[1][1]));

    // 预计算变换矩阵（更精确的椭圆计算）
    mat2 invScale = mat2(
    1.0 / radiusX, 0.0,
    0.0, 1.0 / radiusY
    );

    mat2 rot = mat2(
    dirX.x, dirY.x,
    dirX.y, dirY.y
    );

    mat2 transform = rot * invScale;

    // 确定要处理的像素范围（扩大搜索范围）
    float searchRadius = max(radiusX, radiusY) * 4.0;  // 增大搜索范围
    ivec2 minCoord = ivec2(max(0, int(floor(screenPos.x - searchRadius))),
    max(0, int(floor(screenPos.y - searchRadius))));
    ivec2 maxCoord = ivec2(min(params.width - 1, int(ceil(screenPos.x + searchRadius))),
    min(params.height - 1, int(ceil(screenPos.y + searchRadius))));

    // 对每个受影响的像素应用高斯混合
    for (int y = minCoord.y; y <= maxCoord.y; y++) {
        for (int x = minCoord.x; x <= maxCoord.x; x++) {
            // 计算像素到点中心的距离向量
            vec2 pixelOffset = vec2(x, y) - screenPos;

            // 应用变换（椭圆形状）
            vec2 transformedOffset = transform * pixelOffset;

            // 计算椭圆距离
            float distanceSquared = dot(transformedOffset, transformedOffset);

            // 应用高斯权重（减小衰减系数，使影响范围更广）
            float gaussianWeight = exp(-distanceSquared * 2.0);

            // 应用不透明度
            float alpha = gaussianWeight * opacity;

            // 降低阈值，使边缘更柔和
            if (alpha > 0.0001) {
                uint pixelIdx = y * params.width + x;
                vec4 pixelColor = unpackColor(pixelMap.pixels[pixelIdx]);

                // Alpha混合
                vec4 newColor = mix(pixelColor, color * alpha, alpha);
                newColor.a = clamp(pixelColor.a + alpha * (1.0 - pixelColor.a), 0.0, 1.0);

                pixelMap.pixels[pixelIdx] = packColor(newColor);
            }
        }
    }
}
