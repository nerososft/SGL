//
// Created by 1234 on 2025/3/6.
//

#include "BaseFilter.h"

#include <iostream>
#include <ostream>
#include <vulkan/vk_enum_string_helper.h>

#include "effect_engine/gpu/VkGPUComputePipeline.h"
#include "effect_engine/gpu/VkGPUDescriptorSet.h"
#include "effect_engine/gpu/VkGPUHelper.h"

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
        return ret;
    }

    auto pipelineDescriptorSet = VkGPUDescriptorSet(gpuCtx->GetCurrentDevice(),
                                                    computePipeline.GetPipelineLayout(),
                                                    computePipeline.GetDescriptorSetLayout());
    ret = pipelineDescriptorSet.AllocateDescriptorSets(gpuCtx->GetDescriptorPool());
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to allocate descriptor sets, err =" << string_VkResult(ret) << std::endl;
        return ret;
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

    VkFence computeFence = VK_NULL_HANDLE;
    ret = VkGPUHelper::CreateFence(gpuCtx->GetCurrentDevice(), &computeFence);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to create compute fence, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    ret = VkGPUHelper::AllocateCommandBuffers(gpuCtx->GetCurrentDevice(), gpuCtx->GetCommandPool(), 1, &commandBuffer);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to allocate command buffer, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    vkResetFences(gpuCtx->GetCurrentDevice(), 1, &computeFence);

    uint32_t groupCountX = (width + 15) / 16;
    uint32_t groupCountY = (height + 15) / 16;
    uint32_t groupCountZ = 0;

    VkGPUHelper::GPUBeginCommandBuffer(commandBuffer);
    computePipeline.GPUCmdBindPipeline(commandBuffer);
    pipelineDescriptorSet.GPUCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE);
    VkGPUHelper::GPUCmdPushConstant(commandBuffer,
                                    computePipeline.GetPipelineLayout(),
                                    VK_SHADER_STAGE_COMPUTE_BIT,
                                    0,
                                    filterParams.paramsSize,
                                    filterParams.paramsData);
    VkGPUHelper::GPUCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
    std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers;

    VkBufferMemoryBarrier bufferMemoryBarrier = {};
    bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    bufferMemoryBarrier.pNext = nullptr;
    bufferMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    bufferMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    bufferMemoryBarrier.buffer = outputBuffer;
    bufferMemoryBarrier.offset = 0;
    bufferMemoryBarrier.size = bufferSize;
    bufferMemoryBarrier.dstQueueFamilyIndex = 0;
    bufferMemoryBarrier.srcQueueFamilyIndex = 0;

    bufferMemoryBarriers.push_back(bufferMemoryBarrier);
    VkGPUHelper::GPUCmdPipelineBufferMemBarrier(commandBuffer,
                                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                0,
                                                bufferMemoryBarriers);
    VkGPUHelper::GPUEndCommandBuffer(commandBuffer);

    std::vector<VkCommandBuffer> submitCommandBuffers;
    submitCommandBuffers.push_back(commandBuffer);

    std::vector<VkSemaphore> submitSignalSemaphores;
    std::vector<VkSemaphore> submitWaitSemaphores;

    VkPipelineStageFlags submitWaitDstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

    std::vector<VkSubmitInfo> submitInfos;
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = submitCommandBuffers.size();
    submitInfo.pCommandBuffers = submitCommandBuffers.data();
    submitInfo.signalSemaphoreCount = submitSignalSemaphores.size();
    submitInfo.pSignalSemaphores = submitSignalSemaphores.data();
    submitInfo.waitSemaphoreCount = submitWaitSemaphores.size();
    submitInfo.pWaitSemaphores = submitWaitSemaphores.data();
    submitInfo.pWaitDstStageMask = &submitWaitDstStageMask;
    submitInfos.push_back(submitInfo);

    ret = VkGPUHelper::GPUQueueSubmit(gpuCtx->GetQueue(), submitInfos, computeFence);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to submit command buffer, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    ret = vkWaitForFences(gpuCtx->GetCurrentDevice(), 1, &computeFence, VK_TRUE, UINT64_MAX);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to wait fence, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }

    ret = vkQueueWaitIdle(gpuCtx->GetQueue());
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to wait idle, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}


VkResult BaseFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           VkDeviceSize bufferSize,
                           uint32_t width,
                           uint32_t height,
                           VkBuffer inputBuffer,
                           VkBuffer outputBuffer) {
    return VK_SUCCESS;
}
