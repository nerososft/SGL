#version 450

layout (location = 0) in vec3 color;
layout (location = 2) in vec3 position;

layout (location = 0) out vec4 FragColor;

layout (push_constant) uniform FrameInfo {
    uint frameIndex;
    float time;
} frameInfo;

void main() {
    FragColor = vec4(color,1.0f);
}