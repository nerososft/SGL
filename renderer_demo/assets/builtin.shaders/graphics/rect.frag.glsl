#version 450

layout(location = 0) in vec3 color;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 position;

layout(location = 0) out vec3 FragColor;

layout (binding = 0) uniform Material {
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    vec3 transparentColor;
    vec3 emissiveColor;
    vec3 reflectiveColor;
    vec3 shininess;
} material;

layout (push_constant) uniform FrameInfo {
    uint frameIndex;
    float time;
} frameInfo;

vec3 calculatePhongLighting(vec3 lightPos, vec3 viewPos, vec3 norm, vec3 fragPos) {
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 ambient = material.ambientColor;
    float diffuseFactor = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseFactor * material.diffuseColor;
    float specularFactor = pow(max(dot(viewDir, reflectDir), 0), material.shininess.r);
    vec3 specular = specularFactor * material.specularColor;

    return ambient + diffuse + specular;
}

void main() {
    vec3 lightPos = vec3(-3, -3, -3);
    vec3 viewPos = vec3(0, 0, -3);
    vec3 norm = normalize(normal);

    vec3 phongLighting = calculatePhongLighting(lightPos, viewPos, norm, position);
    vec3 finalColor = vec3(color * phongLighting);
    FragColor = vec3(finalColor.b, finalColor.g, finalColor.r); // surface not support rgb
}