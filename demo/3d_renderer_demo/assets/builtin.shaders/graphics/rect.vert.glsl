#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outPosition;

layout (binding = 1) uniform ModelMatrix {
    mat4 model;
} modelMatrix;

layout (binding = 2) uniform ViewProjectionMatrix {
    mat4 view;
    mat4 projection;
    vec3 cameraPosition;
    vec3 up;
} vpMatrix;

void main() {
    // 计算世界空间位置 (修复：使用model矩阵转换顶点位置)
    vec4 worldPos = modelMatrix.model * vec4(position, 1.0);
    // 计算裁剪空间位置
    vec4 clipPos = vpMatrix.projection * vpMatrix.view * worldPos;

    gl_Position = clipPos;
    outColor = color;
    outPosition = worldPos.xyz;// 传递世界空间位置给片段着色器

    // 计算世界空间法线 (修复：使用逆转置矩阵正确变换法线)
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix.model)));
    outNormal = normalize(normalMatrix * normal);
}