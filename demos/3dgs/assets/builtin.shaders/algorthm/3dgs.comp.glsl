#version 450
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct GaussianPoint3D {
    vec4 pos;
    vec4 color;
    vec4 scale;
    vec4 rotate;
    vec4 opacity;
};

layout(set = 0, binding = 0) readonly buffer InputPoints {
    GaussianPoint3D points[];
} inputPoints;

layout(set = 0, binding = 1) buffer PixelMap {
    uint pixels[];
} pixelMap;

layout(push_constant) uniform Params {
    mat4 view;
    mat4 proj;
    uint numPoints;
    uint width;
    uint height;
} params;

uint packColor(vec4 color) {
    return (
    uint(clamp(color.a, 0.0, 1.0) * 255.0) << 24) |
    (uint(clamp(color.b, 0.0, 1.0) * 255.0) << 16) |
    (uint(clamp(color.g, 0.0, 1.0) * 255.0) << 8) |
    (uint(clamp(color.r, 0.0, 1.0) * 255.0)
    );
}

vec4 unpackColor(uint color) {
    return vec4(
    float((color) & 0xFF) / 255.0f,
    float((color >> 8) & 0xFF) / 255.0f,
    float((color >> 16) & 0xFF) / 255.0f,
    float((color >> 24) & 0xFF) / 255.0f
    );
}

void main() {
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= params.numPoints) return;

    GaussianPoint3D point = inputPoints.points[idx];

    float rx = point.scale.x;
    float ry = point.scale.y;

    vec3 minPixel = vec3(point.pos.x - rx, point.pos.y - ry, 0);
    vec3 maxPixel = vec3(point.pos.x + rx, point.pos.y + ry, 0);
    barrier();// replace whith depth buffer

    for (float y = minPixel.y; y <= maxPixel.y; y++) {
        for (float x = minPixel.x; x <= maxPixel.x; x++) {
            float factorX = abs(point.pos.x - x) / rx;
            float factorY = abs(point.pos.y - y) / ry;

            if ((factorX <= sqrt(1 - factorY * factorY)) &&
                (factorX >= -sqrt(1 - factorY * factorY))) {
                float factor = sqrt(1.0f - (factorX*factorX) - (factorY*factorY));
                pixelMap.pixels[uint(y * params.width + x)] = packColor(point.color * factor);
            } else {
                continue;
            }
        }
    }
}
