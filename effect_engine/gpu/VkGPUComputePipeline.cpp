//
// Created by neo on 2025/3/9.
//

#include "VkGPUComputePipeline.h"

#include <iostream>
#include <ostream>
#include <vulkan/vk_enum_string_helper.h>

#include "VkGPUHelper.h"

VkGPUComputePipeline::VkGPUComputePipeline(const std::string &computeShaderPath,
                                           const std::vector<VkDescriptorSetLayoutBinding> &descriptorSetLayoutBindings,
                                           const std::vector<VkPushConstantRange> &pushConstantRanges) {
    this->computeShaderPath = computeShaderPath;
    this->descriptorSetLayoutBindings = descriptorSetLayoutBindings;
    this->pushConstantRanges = pushConstantRanges;
}

VkResult VkGPUComputePipeline::CreateComputePipeline(const VkDevice device,
                                                     VkPipelineCache pipelineCache) {
    this->device = device;
    VkResult ret = VkGPUHelper::CreateDescriptorSetLayout(device,
                                                          descriptorSetLayoutBindings,
                                                          &this->descriptorSetLayout);
    if (ret != VK_SUCCESS) {
        std::cout << "failed to create descriptor set layout, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    descriptorSetLayouts.push_back(this->descriptorSetLayout);
    ret = VkGPUHelper::CreatePipelineLayout(device,
                                            descriptorSetLayouts,
                                            this->pushConstantRanges,
                                            &this->pipelineLayout);
    if (ret != VK_SUCCESS) {
        std::cout << "failed to create pipeline layout, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    ret = VkGPUHelper::CreateShaderModuleFromPath(device,
                                                  computeShaderPath,
                                                  &this->computeShaderModule);
    if (ret != VK_SUCCESS) {
        std::cout << "failed to create shader module, err=" << string_VkResult(ret) << std::endl;
    }
    return VkGPUHelper::CreateComputePipeline(device,
                                              pipelineCache,
                                              this->pipelineLayout,
                                              this->computeShaderModule,
                                              &this->computePipeline);
}

void VkGPUComputePipeline::GPUCmdBindPipeline(const VkCommandBuffer commandBuffer) const {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->computePipeline);
}
