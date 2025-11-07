#version 450

layout (location = 0) out vec3 outColor;

struct GaussianPoint {
    vec4 position;
    vec4 color;
    vec4 scale;
    vec4 rotate;
    vec4 opacity;
    // Spherical Harmonics
};

layout (std430, binding = 0) buffer InputStorageBuffer {
    GaussianPoint points[];
} inputPoints;

void main() {
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0f);
    outColor = vec3(1.0f, 0.0, 0.0);
}