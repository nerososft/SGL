//
// Created by neo on 2025/3/25.
//

#include "VkGPUGraphicsPipeline.h"

#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "VkGPUHelper.h"
#include "effect_engine/log/Log.h"

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
    this->device = device;
    VkResult ret = VkGPUHelper::CreateDescriptorSetLayout(device,
                                                          descriptorSetLayoutBindings,
                                                          &this->descriptorSetLayout);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to create descriptor set layout, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    descriptorSetLayouts.push_back(this->descriptorSetLayout);
    ret = VkGPUHelper::CreatePipelineLayout(device,
                                            descriptorSetLayouts,
                                            this->pushConstantRanges,
                                            &this->pipelineLayout);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to create pipeline layout, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }

    ret = VkGPUHelper::CreateShaderModuleFromPath(device,
                                                  vertexShaderPath,
                                                  &this->vertexShaderModule);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to create vertex shader module, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }

    ret = VkGPUHelper::CreateShaderModuleFromPath(device,
                                                  fragShaderPath,
                                                  &this->fragmentShaderModule);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to create fragment shader module, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }

    // TODO:
    constexpr std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
    constexpr std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;

    constexpr std::vector<VkViewport> viewports;
    constexpr std::vector<VkRect2D> viewportScissors;

    constexpr std::vector<VkPipelineColorBlendAttachmentState> colorBlendStateCreateInfos;
    constexpr std::vector<VkDynamicState> dynamicStates;

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
                                                 viewports,
                                                 viewportScissors,
                                                 VK_POLYGON_MODE_FILL,
                                                 1.0f,
                                                 colorBlendStateCreateInfos,
                                                 dynamicStates,
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
