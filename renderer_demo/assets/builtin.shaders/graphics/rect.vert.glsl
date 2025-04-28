#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outPosition;

layout (binding = 1) uniform MVP {
     mat4 model;
     mat4 view;
     mat4 projection;
} mvp;

void main() {
    vec4 pos = mvp.projection * mvp.view * mvp.model * vec4(position, 1.0f);
    gl_Position = pos;
    outNormal = normal;
    outColor = color;
    outPosition = pos.xyz;
}