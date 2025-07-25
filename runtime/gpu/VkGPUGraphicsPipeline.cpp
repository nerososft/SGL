//
// Created by neo on 2025/3/25.
//

#include "VkGPUGraphicsPipeline.h"

#ifdef OS_OPEN_HARMONY
#include <core/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "VkGPUHelper.h"
#include "log/Log.h"

VkGPUGraphicsPipeline::VkGPUGraphicsPipeline(
    const std::string &vertexShaderPath, const std::string &fragShaderPath,
    const float viewWidth, const float viewHeight,
    const std::vector<VkDescriptorSetLayoutBinding>
        &descriptorSetLayoutBindings,
    const std::vector<VkPushConstantRange> &pushConstantRanges,
    const std::vector<VkVertexInputBindingDescription>
        &vertexInputBindingDescriptions,
    const std::vector<VkVertexInputAttributeDescription>
        &vertexInputAttributeDescriptions) {
  this->vertexShaderPath = vertexShaderPath;
  this->fragShaderPath = fragShaderPath;
  this->viewWidth = viewWidth;
  this->viewHeight = viewHeight;
  this->descriptorSetLayoutBindings = descriptorSetLayoutBindings;
  this->pushConstantRanges = pushConstantRanges;
  this->vertexInputBindingDescriptions = vertexInputBindingDescriptions;
  this->vertexInputAttributeDescriptions = vertexInputAttributeDescriptions;
}

VkResult VkGPUGraphicsPipeline::CreateGraphicsPipeline(
    const VkDevice device, const VkPipelineCache pipelineCache,
    const VkRenderPass renderPass) {
  VkResult result = VK_SUCCESS;
  this->device = device;
  VkResult ret = VkGPUHelper::CreateDescriptorSetLayout(
      device, descriptorSetLayoutBindings, &this->descriptorSetLayout);
  if (ret != VK_SUCCESS) {
    Logger() << "failed to create descriptor set layout, err="
             << string_VkResult(ret) << std::endl;
    return ret;
  }

  std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
  descriptorSetLayouts.push_back(this->descriptorSetLayout);
  ret = VkGPUHelper::CreatePipelineLayout(device, descriptorSetLayouts,
                                          this->pushConstantRanges,
                                          &this->pipelineLayout);
  if (ret != VK_SUCCESS) {
    Logger() << "failed to create pipeline layout, err=" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  ret = VkGPUHelper::CreateShaderModuleFromSpvPath(device, vertexShaderPath,
                                                   &this->vertexShaderModule);
  if (ret != VK_SUCCESS) {
    Logger() << "failed to create vertex shader module, err="
             << string_VkResult(ret) << std::endl;
    return ret;
  }

  ret = VkGPUHelper::CreateShaderModuleFromSpvPath(device, fragShaderPath,
                                                   &this->fragmentShaderModule);
  if (ret != VK_SUCCESS) {
    Logger() << "failed to create fragment shader module, err="
             << string_VkResult(ret) << std::endl;
    return ret;
  }

  std::vector<VkViewport> viewports;
  VkViewport viewport;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = viewWidth;
  viewport.height = viewHeight;
  viewports.push_back(viewport);
  std::vector<VkRect2D> viewportScissors;
  VkRect2D scissor;
  scissor.offset = {0, 0};
  scissor.extent.width = static_cast<uint32_t>(viewWidth);
  scissor.extent.height = static_cast<uint32_t>(viewHeight);
  viewportScissors.push_back(scissor);

  std::vector<VkPipelineColorBlendAttachmentState> colorBlendStateCreateInfos;
  const VkPipelineColorBlendAttachmentState colorBlendAttachment = {
      // FIXME: should be dynamic from attahcments
      .blendEnable = VK_FALSE,
      .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
      .alphaBlendOp = VK_BLEND_OP_ADD,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };
  colorBlendStateCreateInfos.push_back(colorBlendAttachment);
  const std::vector<VkDynamicState> dynamicStates;

  result = VkGPUHelper::CreateGraphicsPipeline(
      device, pipelineCache, this->pipelineLayout, this->vertexShaderModule,
      this->fragmentShaderModule, renderPass, vertexInputBindingDescriptions,
      vertexInputAttributeDescriptions, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      VK_FALSE, viewports, viewportScissors, VK_POLYGON_MODE_FILL, 1.0f,
      colorBlendStateCreateInfos, dynamicStates, &this->graphicsPipeline);
  return result;
}

void VkGPUGraphicsPipeline::GPUCmdBindPipeline(
    const VkCommandBuffer commandBuffer) const {
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    this->graphicsPipeline);
}

void VkGPUGraphicsPipeline::Destroy() const {
  if (this->graphicsPipeline != VK_NULL_HANDLE) {
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
  }
  if (this->vertexShaderModule != VK_NULL_HANDLE) {
    vkDestroyShaderModule(device, vertexShaderModule, nullptr);
  }
  if (this->fragmentShaderModule != VK_NULL_HANDLE) {
    vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
  }
  if (this->descriptorSetLayout != VK_NULL_HANDLE) {
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
  }
  if (this->pipelineLayout != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
  }
}
