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

// 四元数转旋转矩阵
mat3 quatToMat3(vec4 q) {
    float qxx = q.x * q.x;
    float qyy = q.y * q.y;
    float qzz = q.z * q.z;
    float qxy = q.x * q.y;
    float qxz = q.x * q.z;
    float qyz = q.y * q.z;
    float qwx = q.w * q.x;
    float qwy = q.w * q.y;
    float qwz = q.w * q.z;

    return mat3(
    1.0 - 2.0 * (qyy + qzz),
    2.0 * (qxy + qwz),
    2.0 * (qxz - qwy),
    2.0 * (qxy - qwz),
    1.0 - 2.0 * (qxx + qzz),
    2.0 * (qyz + qwx),
    2.0 * (qxz + qwy),
    2.0 * (qyz - qwx),
    1.0 - 2.0 * (qxx + qyy)
    );
}

// 从缩放和旋转计算协方差矩阵
mat3 computeCovarianceMatrix(vec4 scale, vec4 rotate) {
    // 提取缩放值
    vec3 s = scale.xyz;

    // 缩放矩阵
    mat3 S = mat3(
    s.x * s.x, 0.0, 0.0,
    0.0, s.y * s.y, 0.0,
    0.0, 0.0, s.z * s.z
    );

    // 旋转矩阵
    mat3 R = quatToMat3(rotate);

    // 计算协方差矩阵: Sigma = R * S * R^T
    mat3 R_transposed = transpose(R);
    mat3 covariance = R * S * R_transposed;

    return covariance;
}

// 计算2D高斯分布值（投影到屏幕空间）
float gaussian2D(vec2 x, vec2 mu, mat2 sigma) {
    // 计算协方差矩阵的行列式
    float det = determinant(sigma);

    // 计算协方差矩阵的逆
    mat2 sigmaInv = inverse(sigma);

    // 计算x - mu
    vec2 diff = x - mu;

    // 计算 (x - mu)^T * Sigma^-1 * (x - mu)
    float exponent = dot(diff, sigmaInv * diff);

    // 计算完整的高斯公式（2D版本）
    float coefficient = 1.0 / (2.0 * 3.14159265358979323846 * sqrt(det));
    float result = coefficient * exp(-0.5 * exponent);

    return result;
}

// 从3D协方差矩阵计算2D投影的协方差矩阵
mat2 computeProjectedCovariance(mat3 cov3D, vec3 viewDir) {
    // 提取协方差矩阵的前两行（忽略z分量）
    // 这是一个简化的近似，实际实现中应该更精确地处理投影
    return mat2(
    cov3D[0][0], cov3D[0][1],
    cov3D[1][0], cov3D[1][1]
    );
}

void main() {
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= params.numPoints) return;

    GaussianPoint3D point = inputPoints.points[idx];
    vec3 position = point.position.xyz;
    vec4 color = point.color;
    vec4 scale = point.scale;
    vec4 rotation = point.rotate;
    float opacity = point.opacity.x;

    if (any(isnan(position))
    || any(isinf(position))
    || any(isnan(scale))
    || any(isinf(scale))
    || any(isnan(rotation))
    || any(isinf(rotation))
    || isnan(opacity)
    || isinf(opacity)) {
        return;
    }

    // 应用视图变换
    vec4 viewPos = params.view * vec4(position, 1.0);

    // 检查点是否在相机前方
    if (viewPos.z >= 0.0) {
        return; // 在相机后方，不可见
    }

    // 应用投影变换
    vec4 clipPos = params.proj * viewPos;

    // 透视除法得到NDC坐标
    vec3 ndcPos = clipPos.xyz / clipPos.w;

    // 检查点是否在视锥体内
    if (abs(ndcPos.x) > 1.0 || abs(ndcPos.y) > 1.0 || abs(ndcPos.z) > 1.0) {
        return;
    }

    // 转换到屏幕空间坐标 (注意：+0.5 是为了对齐像素中心)
    vec2 screenPos = vec2(
    (ndcPos.x * 0.5 + 0.5) * float(params.width - 1) + 0.5,
    (1.0 - (ndcPos.y * 0.5 + 0.5)) * float(params.height - 1) + 0.5
    );

    // 计算3D协方差矩阵
    mat3 cov3D = computeCovarianceMatrix(scale, rotation);

    // 计算2D投影的协方差矩阵（简化版本）
    mat2 cov2D = computeProjectedCovariance(cov3D, normalize(-viewPos.xyz));

    // 计算影响半径（基于协方差矩阵的特征值）
    float det = determinant(cov2D);
    float trace = cov2D[0][0] + cov2D[1][1];
    float radius  = 8.0 * sqrt(trace); // 简化计算，使用迹作为方差的估计

    // 确保半径至少为1像素
    radius = max(radius, 1.0);

    // 计算要处理的像素范围
    ivec2 minPixel = ivec2(max(floor(screenPos - radius), vec2(0.0)));
    ivec2 maxPixel = ivec2(min(ceil(screenPos + radius), vec2(params.width - 1, params.height - 1)));

    // 获取当前点的深度值
    float pointDepth = -viewPos.z; // 使用负z是因为viewPos的z是负值（相机朝向-z）

    // 处理影响范围内的像素
    for (int y = minPixel.y; y <= maxPixel.y; y++) {
        for (int x = minPixel.x; x <= maxPixel.x; x++) {
            uint pixelIdx = y * params.width + x;

            // 计算像素中心到高斯点中心的距离
            vec2 pixelCenter = vec2(x + 0.5, y + 0.5);

            // 计算2D高斯权重
            float weight = gaussian2D(pixelCenter, screenPos, cov2D);

            // 如果权重太小，跳过此像素以提高性能
            if (weight < 0.001) continue;

            // 透明度混合
            vec4 pixelColor = unpackColor(pixelMap.pixels[pixelIdx]);
            float pixelDepth = depthMap.depth[pixelIdx];

            // 深度测试
            if (pointDepth < pixelDepth) {
                // 计算混合后的颜色
                vec4 blendedColor = mix(pixelColor, color * opacity, weight);

                // 更新像素颜色和深度
                pixelMap.pixels[pixelIdx] = packColor(blendedColor);
                depthMap.depth[pixelIdx] = pointDepth;
            }
        }
    }
}