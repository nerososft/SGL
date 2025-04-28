#version 450

layout(location = 0) in vec3 color;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 position;

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
    vec3 lightPos = vec3(3, 3, 3);
    vec3 viewPos = vec3(0, 0, -3);

    vec3 ambientColor = vec3(0.2f, 0.2f, 0.2f);
    vec3 diffuseColor = vec3(0.3f, 0.3f, 0.3f);
    vec3 specularColor = vec3(0.7f, 0.7f, 0.7f);

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - position);
    vec3 ambient = ambientColor;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor;
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2.0);
    vec3 specular = spec * specularColor;
    FragColor = ambient + diffuse + specular;
}