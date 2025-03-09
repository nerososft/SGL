//
// Created by 1234 on 2025/3/6.
//

#include "BaseFilter.h"

#include <iostream>
#include <ostream>
#include <vulkan/vk_enum_string_helper.h>

#include "effect_engine/gpu/VkGPUComputePipeline.h"
#include "effect_engine/gpu/VkGPUDescriptorSet.h"

VkResult BaseFilter::DoApply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                             VkDeviceSize bufferSize,
                             uint32_t width,
                             uint32_t height,
                             VkBuffer inputBuffer,
                             VkBuffer outputBuffer,
                             const FilterParams &filterParams) {
    const std::string computeShaderPath = filterParams.shaderPath;
    const uint32_t paramsSize = filterParams.paramsSize;
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    VkDescriptorSetLayoutBinding inputImageBinding;
    inputImageBinding.binding = 0;
    inputImageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    inputImageBinding.descriptorCount = 1;
    inputImageBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    descriptorSetLayoutBindings.push_back(inputImageBinding);
    VkDescriptorSetLayoutBinding outputImageBinding;
    outputImageBinding.binding = 1;
    outputImageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    outputImageBinding.descriptorCount = 1;
    outputImageBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    descriptorSetLayoutBindings.push_back(outputImageBinding);
    std::vector<VkPushConstantRange> pushConstantRanges;
    VkPushConstantRange pushConstantRange;
    pushConstantRange.offset = 0;
    pushConstantRange.size = paramsSize;
    pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pushConstantRanges.push_back(pushConstantRange);
    VkGPUComputePipeline computePipeline(computeShaderPath, descriptorSetLayoutBindings, pushConstantRanges);
    VkResult ret = computePipeline.CreateComputePipeline(gpuCtx->GetCurrentDevice(),
                                                         gpuCtx->GetPipelineCache());
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to create compute pipeline, err =" << string_VkResult(ret) << std::endl;
    }

    auto pipelineDescriptorSet = VkGPUDescriptorSet(gpuCtx->GetCurrentDevice(),
                                                    computePipeline.GetPipelineLayout(),
                                                    computePipeline.GetDescriptorSetLayout());
    ret = pipelineDescriptorSet.AllocateDescriptorSets(gpuCtx->GetDescriptorPool());
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to allocate descriptor sets, err =" << string_VkResult(ret) << std::endl;
    }
    VkDescriptorBufferInfo inputImageBufferInfo = {};
    inputImageBufferInfo.offset = 0;
    inputImageBufferInfo.range = bufferSize;
    inputImageBufferInfo.buffer = inputBuffer;
    pipelineDescriptorSet.AddStorageBufferDescriptorSet(0, inputImageBufferInfo);
    VkDescriptorBufferInfo outputImageBufferInfo = {};
    outputImageBufferInfo.offset = 0;
    outputImageBufferInfo.range = bufferSize;
    outputImageBufferInfo.buffer = outputBuffer;
    pipelineDescriptorSet.AddStorageBufferDescriptorSet(1, outputImageBufferInfo);

    // TODO: record command
}


VkResult BaseFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           VkDeviceSize bufferSize,
                           uint32_t width,
                           uint32_t height,
                           VkBuffer inputBuffer,
                           VkBuffer outputBuffer) {
    return VK_SUCCESS;
}
