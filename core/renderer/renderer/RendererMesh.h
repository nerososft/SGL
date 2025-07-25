//
// Created by neo on 25-4-27.
//

#ifndef MODEL_H
#define MODEL_H
#include <glm/glm.hpp>
#include <memory>

#include "examples/renderer_demo/scene/Scene.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/compute_graph/IComputeGraphNode.h"

struct ModelMatrix {
  glm::mat4 model;
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
               const std::vector<uint32_t> &indices, const Material &material,
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

#endif // MODEL_H
