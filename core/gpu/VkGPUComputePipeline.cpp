//
// Created by neo on 2025/3/9.
//

#include "VkGPUComputePipeline.h"

#include <iostream>
#include <ostream>
#ifdef OS_OPEN_HARMONY
#include <core/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "ComputePipelineCache.h"
#include "VkGPUHelper.h"
#include "log/Log.h"

VkGPUComputePipeline::VkGPUComputePipeline(
    const std::string &computeShaderPath,
    const std::vector<VkDescriptorSetLayoutBinding>
        &descriptorSetLayoutBindings,
    const std::vector<VkPushConstantRange> &pushConstantRanges) {
  this->computeShaderPath = computeShaderPath;
  this->descriptorSetLayoutBindings = descriptorSetLayoutBindings;
  this->pushConstantRanges = pushConstantRanges;
}

VkResult VkGPUComputePipeline::CreateComputePipeline(
    const VkDevice device, const VkPipelineCache pipelineCache) {
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

  const VkShaderModule cachedModule =
      ComputePipelineCache::GetInstance()->GetShaderModule(computeShaderPath);
  if (cachedModule != VK_NULL_HANDLE) {
    this->computeShaderModule = cachedModule;
  } else {
    ret = VkGPUHelper::CreateShaderModuleFromPath(device, computeShaderPath,
                                                  &this->computeShaderModule);
    if (ret != VK_SUCCESS) {
      Logger() << "failed to create shader module, err=" << string_VkResult(ret)
               << std::endl;
      return ret;
    }
    ComputePipelineCache::GetInstance()->CacheShaderModule(
        this->computeShaderPath, this->computeShaderModule);
  }

  const VkPipeline cachedPipeline =
      ComputePipelineCache::GetInstance()->GetComputePipeline(
          computeShaderPath);
  if (cachedPipeline != VK_NULL_HANDLE) {
    this->computePipeline = cachedPipeline;
    return VK_SUCCESS;
  }

  ret = VkGPUHelper::CreateComputePipeline(
      device, pipelineCache, this->pipelineLayout, this->computeShaderModule,
      &this->computePipeline);
  if (ret != VK_SUCCESS) {
    Logger() << "failed to create pipeline, err=" << string_VkResult(ret)
             << std::endl;
    return ret;
  }
  ComputePipelineCache::GetInstance()->CacheComputePipeline(
      this->computeShaderPath, this->computePipeline);
  return VK_SUCCESS;
}

void VkGPUComputePipeline::GPUCmdBindPipeline(
    const VkCommandBuffer commandBuffer) const {
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                    this->computePipeline);
}

void VkGPUComputePipeline::Destroy() {
  if (this->computePipeline != VK_NULL_HANDLE) {
    this->computePipeline = VK_NULL_HANDLE;
  }
  if (this->computeShaderModule != VK_NULL_HANDLE) {
    this->computeShaderModule = VK_NULL_HANDLE;
  }
  if (this->descriptorSetLayout != VK_NULL_HANDLE) {
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    this->descriptorSetLayout = VK_NULL_HANDLE;
  }
  if (this->pipelineLayout != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    this->pipelineLayout = VK_NULL_HANDLE;
  }
}
