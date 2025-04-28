#version 450

layout(location = 0) in vec3 color;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec3 FragColor;

layout (binding = 0) uniform Material {
     vec3 ambientColor;
     vec3 diffuseColor;
     vec3 specularColor;
} material;

layout (push_constant) uniform FrameInfo {
    uint frameIndex;
    float timestamp;
} frameInfo;

void main() {
    vec3 lightPos = vec3(0, 3, 3);
    vec3 viewPos = vec3(0, 0, -3);

    vec3 ambientColor = vec3(0.5255, 0.4314, 0.0314);
    vec3 diffuseColor = vec3(0.5255, 0.4314, 0.0314);
    vec3 specularColor = vec3(0.3500, 0.3500, 0.3500);

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - color);
    vec3 ambient = ambientColor;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor;
    vec3 viewDir = normalize(viewPos - color);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * specularColor;
    FragColor = ambient + diffuse + specular;
}