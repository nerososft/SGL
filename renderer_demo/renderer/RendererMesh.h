//
// Created by neo on 25-4-27.
//

#ifndef MODEL_H
#define MODEL_H
#include <memory>
#include <glm/glm.hpp>

#include "gpu_engine/gpu/VkGPUBuffer.h"
#include "gpu_engine/gpu/compute_graph/IComputeGraphNode.h"

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

struct ModelMatrix {
    glm::mat4 model;
};

struct Mesh {
    std::vector<Vertex> vertexData;
    std::vector<uint32_t> indicesData;
    Material material{};
    glm::mat4 transform{};
};

class RendererMesh {
public:
    Mesh mesh{};

    std::shared_ptr<VkGPUBuffer> vertexBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> indicesBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> materialBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> transformMatrixBuffer = nullptr;

    RendererMesh() = delete;

    RendererMesh(const std::vector<Vertex> &vertices,
                 const std::vector<uint32_t> &indices,
                 const Material &material,
                 const glm::mat4 &transform);

    void SetMaterial(const Material &material);

    void SetTransformMatrix(const glm::mat4 &transform);

    [[nodiscard]] PipelineNodeBuffer GetVertexBufferNode() const;

    [[nodiscard]] PipelineNodeBuffer GetIndicesBufferNode() const;

    [[nodiscard]] PipelineNodeBuffer GetMaterialBufferNode() const;

    [[nodiscard]] PipelineNodeBuffer GetTransformMatrixBufferNode() const;

    bool CreateGPUMesh(const std::shared_ptr<VkGPUContext> &gpuCtx);

    void Destroy();

    ~RendererMesh() = default;
};


#endif //MODEL_H
