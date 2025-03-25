//
// Created by neo on 2025/3/25.
//

#include "VkGPUGraphicsPipeline.h"

#include "VkGPUHelper.h"

VkGPUGraphicsPipeline::VkGPUGraphicsPipeline(const std::string &vertexShaderPath,
                                             const std::string &fragShaderPath,
                                             const std::vector<VkDescriptorSetLayoutBinding> &
                                             descriptorSetLayoutBindings,
                                             const std::vector<VkPushConstantRange> &pushConstantRanges) {
    this->vertexShaderPath = vertexShaderPath;
    this->fragShaderPath = fragShaderPath;
    this->descriptorSetLayoutBindings = descriptorSetLayoutBindings;
    this->pushConstantRanges = pushConstantRanges;
}

VkResult VkGPUGraphicsPipeline::CreateGraphicsPipeline(const VkDevice device,
                                                       const VkPipelineCache pipelineCache,
                                                       const VkRenderPass renderPass) {
    VkResult result = VK_SUCCESS;

    // TODO:
    constexpr std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
    constexpr std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;

    result = VkGPUHelper::CreateGraphicsPipeline(device,
                                                 pipelineCache,
                                                 this->pipelineLayout,
                                                 this->vertexShaderModule,
                                                 this->fragmentShaderModule,
                                                 renderPass,
                                                 vertexInputBindingDescriptions,
                                                 vertexInputAttributeDescriptions,
                                                 VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                 VK_TRUE,
                                                 &this->graphicsPipeline);
    return result;
}

void VkGPUGraphicsPipeline::GPUCmdBindPipeline(const VkCommandBuffer commandBuffer) const {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicsPipeline);
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
