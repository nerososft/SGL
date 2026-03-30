#version 450

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec4 scale;
layout (location = 3) in vec4 rotate;
layout (location = 4) in vec4 opacity;

layout (location = 0) out vec4 outColor;
layout (location = 1) out float outOpacity;

layout (set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    vec4 camPos;
    vec4 renderParams;
} cam;

void main() {
    vec4 viewPos = cam.view * vec4(position.xyz, 1.0f);
    gl_Position = cam.proj * viewPos;

    float worldRadius = max(max(scale.x, scale.y), scale.z);
    float distanceToCamera = max(-viewPos.z, 0.01f);
    float projectedRadius =
        worldRadius * cam.renderParams.x * cam.renderParams.y / distanceToCamera;

    gl_PointSize = clamp(projectedRadius, 1.0f, cam.renderParams.w);

    outColor = color;
    outOpacity = opacity.x;
}
