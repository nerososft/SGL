//
// Created by neo on 2025/3/25.
//

#ifndef GRAPHICSPIPELINENODE_H
#define GRAPHICSPIPELINENODE_H
#include <functional>

#include "IComputeGraphNode.h"
#include "runtime/gpu/VkGPUComputePipeline.h"
#include "runtime/gpu/VkGPUContext.h"
#include "runtime/gpu/VkGPUDescriptorSet.h"
#include "runtime/gpu/VkGPUGraphicsPipeline.h"
#include "runtime/gpu/VkGPURenderPass.h"

struct GraphicsElement {
  PushConstantInfo pushConstantInfo;
  std::vector<PipelineNodeBuffer> buffers;
  std::function<void(VkCommandBuffer commandBuffer)> customDrawFunc;
};

class GraphicsPipelineNode final : public IComputeGraphNode {
  std::string vertexShaderPath;
  std::string fragmentShaderPath;
  std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
  std::vector<VkVertexInputAttributeDescription>
      vertexInputAttributeDescriptions;

  uint32_t pushConstantSize = 0;
  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

  float width = 1.0f;
  float height = 1.0f;

  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
  std::shared_ptr<VkGPURenderPass> renderPass = nullptr;
  std::shared_ptr<VkGPUGraphicsPipeline> graphicsPipeline = nullptr;
  std::vector<std::shared_ptr<VkGPUDescriptorSet>> pipelineDescriptorSets;

  std::vector<GraphicsElement> graphicsElements;

public:
  GraphicsPipelineNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                       const std::string &name,
                       const std::shared_ptr<VkGPURenderPass> &renderPass,
                       const std::string &vertexShaderPath,
                       const std::string &fragmentShaderPath,
                       const uint32_t pushConstantSize,
                       const std::vector<VkDescriptorSetLayoutBinding>
                           descriptorSetLayoutBindings,
                       const std::vector<VkVertexInputBindingDescription>
                           &vertexInputBindingDescriptions,
                       const std::vector<VkVertexInputAttributeDescription>
                           &vertexInputAttributeDescriptions,
                       float width, float height);

  [[nodiscard]] std::shared_ptr<VkGPUDescriptorSet>
  CreateDescriptorSet(const GraphicsElement &graphicsElement) const;

  void AddGraphicsElement(const GraphicsElement &graphicsElement);

  ~GraphicsPipelineNode() override = default;

  VkResult CreateComputeGraphNode() override;

  void Compute(VkCommandBuffer commandBuffer) override;

  void Destroy() override;
};

#endif // GRAPHICSPIPELINENODE_H
