#version 450

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec4 scale;
layout (location = 3) in vec4 rotate;
layout (location = 4) in vec4 opacity;
layout (location = 5) in vec2 quadCoord;

layout (location = 0) out vec4 outColor;
layout (location = 1) out float outOpacity;
layout (location = 2) out vec2 outLocalCoord;

layout (set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    vec4 camPos;
    vec4 renderParams;
} cam;

layout (set = 0, binding = 1, std430) readonly buffer SHBuffer {
    vec4 coeffs[];
} shBuffer;

const float SH_C0 = 0.28209479177387814f;
const float SH_C1 = 0.4886025119029199f;
const float SH_C2[5] = float[](
    1.0925484305920792f,
    -1.0925484305920792f,
    0.31539156525252005f,
    -1.0925484305920792f,
    0.5462742152960396f
);
const float SH_C3[7] = float[](
    -0.5900435899266435f,
    2.890611442640554f,
    -0.4570457994644658f,
    0.3731763325901154f,
    -0.4570457994644658f,
    1.445305721320277f,
    -0.5900435899266435f
);
const uint SH_COEFF_COUNT = 15u;

mat3 quatToMat3(vec4 q) {
    vec4 n = normalize(q);
    float x = n.x;
    float y = n.y;
    float z = n.z;
    float w = n.w;
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    return mat3(
        1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz), 2.0f * (xz - wy),
        2.0f * (xy - wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx),
        2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (xx + yy)
    );
}

vec3 evaluateSH(uint pointIndex, vec3 direction) {
    vec3 dir = normalize(direction);
    float x = dir.x;
    float y = dir.y;
    float z = dir.z;
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float yz = y * z;
    float xz = x * z;

    uint base = pointIndex * SH_COEFF_COUNT;
    vec3 result = SH_C0 * color.rgb;
    result += (-SH_C1 * y) * shBuffer.coeffs[base + 0u].xyz;
    result += ( SH_C1 * z) * shBuffer.coeffs[base + 1u].xyz;
    result += (-SH_C1 * x) * shBuffer.coeffs[base + 2u].xyz;

    result += SH_C2[0] * xy * shBuffer.coeffs[base + 3u].xyz;
    result += SH_C2[1] * yz * shBuffer.coeffs[base + 4u].xyz;
    result += SH_C2[2] * (2.0f * zz - xx - yy) * shBuffer.coeffs[base + 5u].xyz;
    result += SH_C2[3] * xz * shBuffer.coeffs[base + 6u].xyz;
    result += SH_C2[4] * (xx - yy) * shBuffer.coeffs[base + 7u].xyz;

    result += SH_C3[0] * y * (3.0f * xx - yy) * shBuffer.coeffs[base + 8u].xyz;
    result += SH_C3[1] * xy * z * shBuffer.coeffs[base + 9u].xyz;
    result += SH_C3[2] * y * (4.0f * zz - xx - yy) * shBuffer.coeffs[base + 10u].xyz;
    result += SH_C3[3] * z * (2.0f * zz - 3.0f * xx - 3.0f * yy) * shBuffer.coeffs[base + 11u].xyz;
    result += SH_C3[4] * x * (4.0f * zz - xx - yy) * shBuffer.coeffs[base + 12u].xyz;
    result += SH_C3[5] * z * (xx - yy) * shBuffer.coeffs[base + 13u].xyz;
    result += SH_C3[6] * x * (xx - 3.0f * yy) * shBuffer.coeffs[base + 14u].xyz;
    return max(result + vec3(0.5f), vec3(0.0f));
}

void main() {
    vec4 viewPos = cam.view * vec4(position.xyz, 1.0f);
    vec4 clipCenter = cam.proj * viewPos;
    vec2 ndcCenter = clipCenter.xy / max(clipCenter.w, 1e-5f);

    mat3 rotationMat = quatToMat3(rotate);
    mat3 viewRotation = mat3(cam.view) * rotationMat;
    mat3 scaleMat = mat3(
        scale.x, 0.0f, 0.0f,
        0.0f, scale.y, 0.0f,
        0.0f, 0.0f, scale.z
    );
    mat3 transform = viewRotation * scaleMat;
    mat3 covariance3D = transform * transpose(transform);

    float invDepth = 1.0f / max(-viewPos.z, 1e-5f);
    float projX = cam.proj[0][0];
    float projY = cam.proj[1][1];
    vec3 jacobianX = vec3(
        projX * invDepth,
        0.0f,
        projX * viewPos.x * invDepth * invDepth
    );
    vec3 jacobianY = vec3(
        0.0f,
        projY * invDepth,
        projY * viewPos.y * invDepth * invDepth
    );
    vec3 sigmaJacobianX = covariance3D * jacobianX;
    vec3 sigmaJacobianY = covariance3D * jacobianY;
    mat2 covariance = mat2(
        dot(jacobianX, sigmaJacobianX),
        dot(jacobianX, sigmaJacobianY),
        dot(jacobianY, sigmaJacobianX),
        dot(jacobianY, sigmaJacobianY)
    );
    covariance[0][0] += cam.renderParams.y;
    covariance[1][1] += cam.renderParams.y;

    float trace = covariance[0][0] + covariance[1][1];
    float det = covariance[0][0] * covariance[1][1] - covariance[0][1] * covariance[1][0];
    float term = sqrt(max(0.0f, trace * trace * 0.25f - det));
    float lambda0 = max(trace * 0.5f + term, cam.renderParams.y);
    float lambda1 = max(trace * 0.5f - term, cam.renderParams.y);

    vec2 majorDir;
    if (abs(covariance[0][1]) > 1e-8f) {
        majorDir = normalize(vec2(covariance[0][1], lambda0 - covariance[0][0]));
    } else {
        majorDir = covariance[0][0] >= covariance[1][1] ? vec2(1.0f, 0.0f) : vec2(0.0f, 1.0f);
    }
    vec2 minorDir = vec2(-majorDir.y, majorDir.x);

    float extent = cam.renderParams.x;
    vec2 majorAxis = majorDir * min(sqrt(lambda0) * extent, cam.renderParams.w);
    vec2 minorAxis = minorDir * min(sqrt(lambda1) * extent, cam.renderParams.w);
    vec2 quadNdc = ndcCenter + majorAxis * quadCoord.x + minorAxis * quadCoord.y;

    gl_Position = vec4(quadNdc * clipCenter.w, clipCenter.z, clipCenter.w);

    uint pointIndex = uint(gl_VertexIndex) / 4u;
    outColor = vec4(evaluateSH(pointIndex, position.xyz - cam.camPos.xyz), 1.0f);
    outOpacity = 1.0f / (1.0f + exp(-opacity.x));
    outLocalCoord = quadCoord * extent;
}
