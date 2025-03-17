//
// Created by neo on 2025/3/9.
//

#include "VkGPUComputePipeline.h"

#include <iostream>
#include <ostream>
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "VkGPUHelper.h"
#include "effect_engine/log/Log.h"

VkGPUComputePipeline::VkGPUComputePipeline(const std::string &computeShaderPath,
                                           const std::vector<VkDescriptorSetLayoutBinding> &descriptorSetLayoutBindings,
                                           const std::vector<VkPushConstantRange> &pushConstantRanges) {
    this->computeShaderPath = computeShaderPath;
    this->descriptorSetLayoutBindings = descriptorSetLayoutBindings;
    this->pushConstantRanges = pushConstantRanges;
}

VkGPUComputePipeline::~VkGPUComputePipeline() {
    vkDestroyPipeline(device, computePipeline, nullptr);
    vkDestroyShaderModule(device, computeShaderModule, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}

VkResult VkGPUComputePipeline::CreateComputePipeline(const VkDevice device,
                                                     const VkPipelineCache pipelineCache) {
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
                                                  computeShaderPath,
                                                  &this->computeShaderModule);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to create shader module, err=" << string_VkResult(ret) << std::endl;
        return ret;
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
