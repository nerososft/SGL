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

vec2 projectOffset(vec3 viewCenter, vec3 viewOffset) {
    vec4 clip = cam.proj * vec4(viewCenter + viewOffset, 1.0f);
    return clip.xy / max(clip.w, 1e-5f);
}

void main() {
    vec4 viewPos = cam.view * vec4(position.xyz, 1.0f);
    vec4 clipCenter = cam.proj * viewPos;
    vec2 ndcCenter = clipCenter.xy / max(clipCenter.w, 1e-5f);

    mat3 rotationMat = quatToMat3(rotate);
    mat3 viewRotation = mat3(cam.view) * rotationMat;
    vec3 axisX = viewRotation[0] * scale.x;
    vec3 axisY = viewRotation[1] * scale.y;
    vec3 axisZ = viewRotation[2] * scale.z;

    vec2 projectedAxisX = projectOffset(viewPos.xyz, axisX) - ndcCenter;
    vec2 projectedAxisY = projectOffset(viewPos.xyz, axisY) - ndcCenter;
    vec2 projectedAxisZ = projectOffset(viewPos.xyz, axisZ) - ndcCenter;

    mat2 covariance = mat2(
        dot(projectedAxisX, projectedAxisX) + dot(projectedAxisY, projectedAxisY) + dot(projectedAxisZ, projectedAxisZ) + cam.renderParams.y,
        projectedAxisX.x * projectedAxisX.y + projectedAxisY.x * projectedAxisY.y + projectedAxisZ.x * projectedAxisZ.y,
        projectedAxisX.x * projectedAxisX.y + projectedAxisY.x * projectedAxisY.y + projectedAxisZ.x * projectedAxisZ.y,
        projectedAxisX.y * projectedAxisX.y + projectedAxisY.y * projectedAxisY.y + projectedAxisZ.y * projectedAxisZ.y + cam.renderParams.y
    );

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

    outColor = color;
    outOpacity = opacity.x;
    outLocalCoord = quadCoord * extent;
}
