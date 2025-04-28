//
// Created by neo on 25-4-27.
//

#ifndef MODEL_H
#define MODEL_H
#include <memory>

#include "gpu_engine/gpu/VkGPUBuffer.h"
#include "renderer_demo/renderer/Renderer.h"

struct Material {
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    Material material{};

    std::shared_ptr<VkGPUBuffer> vertexBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> indexBuffer = nullptr;

    Mesh() = default;

    Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

    ~Mesh() = default;
};


#endif //MODEL_H
