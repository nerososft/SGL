#version 450

layout(location = 0) in vec3 color;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 position;

layout(location = 0) out vec4 FragColor;

layout (binding = 0) uniform Material {
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    vec3 transparentColor;
    vec3 emissiveColor;
    vec3 reflectiveColor;
    vec3 shininess;
} material;

layout (binding = 3) uniform Light {
    vec3 position;
    vec3 color;
    vec3 direction;
} light;

layout (binding = 2) uniform ViewProjectionMatrix {
    mat4 view;
    mat4 projection;
    vec3 cameraPosition;
    vec3 up;
} vpMatrix;

layout (push_constant) uniform FrameInfo {
    uint frameIndex;
    float time;
} frameInfo;

vec3 calculatePhongLighting(vec3 lightPos, vec3 viewPos, vec3 norm, vec3 fragPos) {
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float ambientStrength = 0.5;
    vec3 ambient = light.color * material.ambientColor * ambientStrength;
    float fakeAO = max(0.3, dot(norm, vec3(0,1,0)));
    ambient *= fakeAO;

    float diffuseFactor = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseFactor * material.diffuseColor;
    float specularFactor = pow(max(dot(viewDir, reflectDir), 0), material.shininess.r);
    vec3 specular = specularFactor * material.specularColor;

    return (ambient + diffuse + specular) * color;
}

void main() {
    vec3 lightPos = light.position;
    vec3 viewPos = vpMatrix.cameraPosition;
    vec3 norm = normalize(normal);

    vec3 phongLighting = calculatePhongLighting(lightPos, viewPos, norm, position);
    FragColor = vec4(phongLighting, 1.0);
}