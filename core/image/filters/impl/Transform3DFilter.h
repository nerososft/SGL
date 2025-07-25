//
// Created by 1234 on 2025/3/6.
//

#ifndef TRANSFORM3DFILTER_H
#define TRANSFORM3DFILTER_H
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

#include "core/image/filters/BasicFilter.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/VkGPUContext.h"
#include "runtime/gpu/compute_graph/GraphicsPipelineNode.h"
#include "runtime/gpu/compute_graph/GraphicsRenderPassNode.h"
#include "runtime/gpu/compute_graph/ImageToBufferCopyNode.h"

struct Vertex {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec2 texCoords;
};

struct TransformFilter3DParams {
  BasicFilterParam imageSize;
  glm::mat4 transformMatrix;
};

class Transform3DFilter final : public BasicFilter {
  TransformFilter3DParams transformFilterParams{};

  uint32_t width = 768;
  uint32_t height = 768;

  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> subComputeGraph = nullptr;

  std::shared_ptr<ImageToBufferCopyNode> offScreenCopyNode = nullptr;

  std::shared_ptr<GraphicsRenderPassNode> mainRenderPassNode = nullptr;
  std::shared_ptr<GraphicsPipelineNode> graphicsPipelineNode = nullptr;
  std::shared_ptr<VkGPUFramebuffer> framebuffer = nullptr;

  std::shared_ptr<VkGPUBuffer> vertexBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> indicesBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> transformMatrixBuffer = nullptr;

  VkImage textureImage = VK_NULL_HANDLE;
  std::shared_ptr<VkGPUBuffer> imageBuffer = nullptr;
  VkImageView textureImageView = VK_NULL_HANDLE;
  VkSampler textureSampler = VK_NULL_HANDLE;

public:
  Transform3DFilter() = default;

  ~Transform3DFilter() override = default;

  [[nodiscard]] PipelineNodeBuffer GetVertexBufferNode() const;

  [[nodiscard]] PipelineNodeBuffer GetIndicesBufferNode() const;

  [[nodiscard]] PipelineNodeBuffer GetTransformMatrixBufferNode() const;

  [[nodiscard]] PipelineNodeBuffer
  GetTextureBufferNode(VkBuffer buffer, VkDeviceMemory memory) const;

  VkResult ConstructMainGraphicsPipeline(const FilterImageInfo &imageInfo);

  VkResult AddDrawElement(const std::vector<Vertex> &vertexData,
                          const std::vector<uint32_t> &indicesData,
                          const glm::mat4 &transform,
                          const FilterImageInfo &imageInfo);

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetTransformMatrix(const glm::mat4 &matrix) {
    this->transformFilterParams.transformMatrix = matrix;
  }

  void Destroy() override;
};

#endif // TRANSFORM3DFILTER_H
