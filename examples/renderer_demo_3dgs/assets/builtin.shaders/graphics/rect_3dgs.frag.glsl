#version 450

layout (location = 0) in vec4 color;
layout (location = 1) in float opacity;

layout (location = 0) out vec4 FragColor;

layout (push_constant) uniform FrameInfo {
    uint frameIndex;
    float time;
} frameInfo;

void main() {
    vec2 uv = gl_PointCoord * 2.0f - 1.0f;
    float radius2 = dot(uv, uv);
    if (radius2 > 1.0f) {
        discard;
    }

    float falloff = exp(-radius2 * 2.5f);
    float alpha = clamp(opacity, 0.03f, 1.0f) * falloff;
    if (alpha < 0.02f) {
        discard;
    }

    vec3 shadedColor = color.rgb * mix(0.6f, 1.0f, falloff);
    FragColor = vec4(shadedColor, alpha);
}
