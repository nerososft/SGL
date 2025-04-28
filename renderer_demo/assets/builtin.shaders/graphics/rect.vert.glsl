#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;

layout(location = 0) out vec3 outColor;

layout (binding = 1) uniform MVP {
     mat4 model;
     mat4 view;
     mat4 projection;
} mvp;

void main() {
    gl_Position = vec4(position, 1.0f);
    outColor = color;
}