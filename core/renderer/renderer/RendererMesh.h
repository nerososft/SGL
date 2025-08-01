//
// Created by neo on 25-4-27.
//

#ifndef MODEL_H
#define MODEL_H
#include <glm/glm.hpp>
#include <memory>

#include "examples/renderer_demo/scene/Scene.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/VkGPUTexture.h"
#include "runtime/gpu/compute_graph/IComputeGraphNode.h"

#include <map>

struct ModelMatrix {
  glm::mat4 model;
};

class RendererMesh {
public:
  std::shared_ptr<Mesh> mesh = nullptr;

  std::shared_ptr<VkGPUBuffer> vertexBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> indicesBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> materialBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> transformMatrixBuffer = nullptr;

  std::map<TextureType, std::shared_ptr<VkGPUTexture>> textures;

  RendererMesh() = delete;

  explicit RendererMesh(const std::shared_ptr<Mesh> &mesh);

  void SetMaterial(const Material &material) const;

  void SetTransformMatrix(const glm::mat4 &transform) const;

  [[nodiscard]] PipelineNodeBuffer GetVertexBufferNode() const;

  [[nodiscard]] PipelineNodeBuffer GetIndicesBufferNode() const;

  [[nodiscard]] PipelineNodeBuffer GetMaterialBufferNode() const;

  [[nodiscard]] PipelineNodeBuffer GetTransformMatrixBufferNode() const;

  PipelineNodeBuffer GetTextureBufferNode(TextureType type) const;

  bool CreateGPUMesh(const std::shared_ptr<VkGPUContext> &gpuCtx);

  void Destroy();

  ~RendererMesh() = default;
};

#endif // MODEL_H
