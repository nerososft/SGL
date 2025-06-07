#version 450
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct GaussianPoint3D {
    vec4 pos;
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

// 2D旋转矩阵
mat2 rotation2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

void main() {
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= params.numPoints) return;

    GaussianPoint3D point = inputPoints.points[idx];

    // 应用视图和投影变换
    vec4 viewPos = params.view * point.pos;
    vec4 clipPos = params.proj * viewPos;
    vec3 ndcPos = clipPos.xyz / clipPos.w;

    if (abs(ndcPos.x) > 1.0 || abs(ndcPos.y) > 1.0 || ndcPos.z < -1.0 || ndcPos.z > 1.0)
    return;

    // 转换到屏幕坐标
    vec2 screenPos = (ndcPos.xy * 0.5 + 0.5) * vec2(params.width, params.height);

    // 获取点的缩放因子（xy方向）
    float scaleX = point.scale.x * max(params.width, params.height) * 0.1;
    float scaleY = point.scale.y * max(params.width, params.height) * 0.1;

    // 计算最大缩放值，用于确定影响范围
    float maxScale = max(scaleX, scaleY);

    // 扩大影响范围，确保完全覆盖边缘衰减
    float influenceRadius = maxScale * 2.0;

    // 确定影响范围
    ivec2 minPixel = ivec2(max(0.0, floor(screenPos.x - influenceRadius)));
    ivec2 maxPixel = ivec2(min(float(params.width - 1), ceil(screenPos.x + influenceRadius)),
    min(float(params.height - 1), ceil(screenPos.y + influenceRadius)));

    // 计算旋转角度（从四元数）
    float rotationAngle = 2.0 * acos(point.rotate.w);

    // 计算旋转矩阵
    mat2 rotMatrix = rotation2D(rotationAngle);

    // 遍历影响范围内的像素
    for (int y = minPixel.y; y <= maxPixel.y; y++) {
        for (int x = minPixel.x; x <= maxPixel.x; x++) {
            // 计算像素到点中心的向量
            vec2 pixelCenter = vec2(x + 0.5, y + 0.5);
            vec2 dir = pixelCenter - screenPos;

            // 应用旋转
            dir = rotMatrix * dir;

            // 应用缩放（椭圆变形）
            dir /= vec2(scaleX, scaleY);

            // 计算椭圆距离
            float dist = length(dir);

            // 计算衰减（使用更陡峭的衰减函数）
            float falloff;

            // 椭圆内部使用高斯函数
            if (dist <= 1.0) {
                falloff = exp(-(dist * dist) / 0.3);
            }
            else {
                continue;// 超出影响范围
            }

            // 最终衰减阈值
            if (falloff < 0.00001) continue;

            // 计算最终颜色
            vec4 pointColor = point.color * point.opacity.x * falloff;
            vec4 pixelColor = unpackColor(pixelMap.pixels[y * params.width + x]);

            // Alpha混合
            vec4 finalColor;
            finalColor.rgb = pixelColor.rgb * (1.0 - pointColor.a) + pointColor.rgb;
            finalColor.a = pixelColor.a + pointColor.a * (1.0 - pixelColor.a);

            pixelMap.pixels[y * params.width + x] = packColor(finalColor);
        }
    }
}
