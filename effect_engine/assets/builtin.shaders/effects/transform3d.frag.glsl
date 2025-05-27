#version 450

layout(location = 0) in vec3 color;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec4 FragColor;

layout (push_constant) uniform FrameInfo {
    uint frameIndex;
} frameInfo;

layout (binding = 1) uniform sampler2D texSampler;

void main() {
    FragColor = texture(texSampler, uv) + vec4(color, 1.0);
}