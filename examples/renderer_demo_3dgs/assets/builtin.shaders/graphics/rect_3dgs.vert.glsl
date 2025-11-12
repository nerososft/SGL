#version 450

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec4 scale;
layout (location = 3) in vec4 rotate;
layout (location = 4) in vec4 opacity;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outPosition;

void main() {
    gl_Position = position;
    outColor = color.rgb;
    outPosition = position.rgb;
}