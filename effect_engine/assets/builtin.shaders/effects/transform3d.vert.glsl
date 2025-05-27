#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;

layout (binding = 0) uniform ModelMatrix {
    mat4 model;
} modelMatrix;

void main() {
    vec4 pos = modelMatrix.model * vec4(position, 1.0f);
    gl_Position = pos;
    outColor = color;
    outTexCoord = texCoord;
}