//
// Created by neo on 2025/3/25.
//

#ifndef VKGPUGRAPHICSPIPELINE_H
#define VKGPUGRAPHICSPIPELINE_H
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

class VkGPUGraphicsPipeline {
  VkDevice device = VK_NULL_HANDLE;
  std::string vertexShaderPath;
  std::string fragShaderPath;
  float viewWidth, viewHeight;

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
  std::vector<VkPushConstantRange> pushConstantRanges;

  std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
  std::vector<VkVertexInputAttributeDescription>
      vertexInputAttributeDescriptions;

  VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
  VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;
  VkPipeline graphicsPipeline = VK_NULL_HANDLE;

public:
  VkGPUGraphicsPipeline(
      const std::string &vertexShaderPath, const std::string &fragShaderPath,
      float viewWidth, float viewHeight,
      const std::vector<VkDescriptorSetLayoutBinding>
          &descriptorSetLayoutBindings,
      const std::vector<VkPushConstantRange> &pushConstantRanges,
      const std::vector<VkVertexInputBindingDescription>
          &vertexInputBindingDescriptions,
      const std::vector<VkVertexInputAttributeDescription>
          &vertexInputAttributeDescriptions);

  ~VkGPUGraphicsPipeline() = default;

  VkResult CreateGraphicsPipeline(VkDevice device,
                                  VkPipelineCache pipelineCache,
                                  VkRenderPass renderPass);

  [[nodiscard]] VkPipelineLayout GetPipelineLayout() const {
    return this->pipelineLayout;
  }

  [[nodiscard]] VkDescriptorSetLayout GetDescriptorSetLayout() const {
    return this->descriptorSetLayout;
  }

  void GPUCmdBindPipeline(VkCommandBuffer commandBuffer) const;

  void Destroy() const;
};

#endif // VKGPUGRAPHICSPIPELINE_H
