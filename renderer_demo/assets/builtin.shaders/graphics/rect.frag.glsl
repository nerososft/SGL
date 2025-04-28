#version 450

layout(location = 0) in vec3 color;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec3 FragColor;

layout (binding = 0) uniform Material {
     vec3 ambientColor;
     vec3 diffuseColor;
     vec3 specularColor;
} material;

layout (push_constant) uniform FrameInfo {
    uint frameIndex;
    float timestamp;
} frameInfo;

void main() {
    FragColor = normal;
}