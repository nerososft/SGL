#version 450

layout (location = 0) in vec4 color;
layout (location = 1) in float opacity;
layout (location = 2) in vec2 localCoord;

layout (location = 0) out vec4 FragColor;

layout (push_constant) uniform FrameInfo {
    uint frameIndex;
    float time;
} frameInfo;

void main() {
    float radius2 = dot(localCoord, localCoord);
    if (radius2 > 9.0f) {
        discard;
    }

    float falloff = exp(-0.5f * radius2);
    float alpha = clamp(opacity, 0.03f, 1.0f) * falloff;
    if (alpha < 0.02f) {
        discard;
    }

    vec3 shadedColor = color.rgb * mix(0.65f, 1.0f, falloff);
    FragColor = vec4(shadedColor, alpha);
}
