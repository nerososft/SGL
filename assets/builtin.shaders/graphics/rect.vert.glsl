#version 450

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 outColor;

void main() {
    gl_Position = vec4(position, 0.0f);
    outColor = vec3(1.0f, 0.0f, 0.0f);
}