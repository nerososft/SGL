//
// Created by neo on 25-5-3.
//

#ifndef SCENE_H
#define SCENE_H
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Material {
    glm::vec4 ambientColor;
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;
    glm::vec4 transparentColor;
    glm::vec4 emissiveColor;
    glm::vec4 reflectiveColor;
    glm::vec4 shininess;
};

struct Mesh {
    std::vector<Vertex> vertexData;
    std::vector<uint32_t> indicesData;
    Material material{};
    glm::mat4 transform{};
};

struct Light {
    glm::vec4 position;
    glm::vec4 color;
    glm::vec4 direction;
};

#endif //SCENE_H
