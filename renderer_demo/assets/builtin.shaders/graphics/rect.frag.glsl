#version 450

layout(location = 0) in vec3 color;
layout(location = 0) out vec3 FragColor;

layout (push_constant) uniform FrameInfo {
    uint frameIndex;
    float timestamp;
} frameInfo;

void main() {
    FragColor = color * sin(frameInfo.frameIndex);
}