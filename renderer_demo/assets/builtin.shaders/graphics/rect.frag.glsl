#version 450

layout(location = 0) in vec3 color;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 position;

layout(location = 0) out vec3 FragColor;

layout (binding = 0) uniform Material {
     vec3 ambientColor;
     vec3 diffuseColor;
     vec3 specularColor;
     float shininess;
} material;

layout (push_constant) uniform FrameInfo {
    uint frameIndex;
    float timestamp;
} frameInfo;

void main() {
    vec3 lightPos = vec3(3, 3, 3);
    vec3 viewPos = vec3(0, 0, -3);

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - position);
    vec3 ambient = material.ambientColor;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuseColor;
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specularColor;
    FragColor = ambient + diffuse + specular;
}