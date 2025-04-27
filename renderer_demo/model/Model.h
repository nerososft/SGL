//
// Created by neo on 25-4-27.
//

#ifndef MODEL_H
#define MODEL_H
#include <memory>

#include "gpu_engine/gpu/VkGPUBuffer.h"
#include "renderer_demo/renderer/Renderer.h"


class Model {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::shared_ptr<VkGPUBuffer> vertexBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> indexBuffer = nullptr;

    Model() = default;

    Model(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

    ~Model() = default;
};


#endif //MODEL_H
