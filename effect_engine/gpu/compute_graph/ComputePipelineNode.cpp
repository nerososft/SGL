//
// Created by neo on 2025/3/11.
//

#include "ComputePipelineNode.h"

#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/gpu/VkGPUComputePipeline.h"
#include "effect_engine/gpu/VkGPUDescriptorSet.h"
#include "effect_engine/gpu/VkGPUHelper.h"
#include "effect_engine/log/Log.h"

ComputePipelineNode::ComputePipelineNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                         const std::string &name,
                                         const std::string &shaderPath,
                                         const uint32_t workGroupCountX,
                                         const uint32_t workGroupCountY,
                                         const uint32_t workGroupCountZ) {
    this->gpuCtx = gpuCtx;
    this->name = name;
    this->type = COMPUTE_GRAPH_NODE_COMPUTE;
    this->shaderPath = shaderPath;
    this->workGroupCountX = workGroupCountX;
    this->workGroupCountY = workGroupCountY;
    this->workGroupCountZ = workGroupCountZ;
}

void ComputePipelineNode::AddComputeElement(const ComputeElement &computeElement) {
    this->computeElements.push_back(computeElement);
}

VkResult ComputePipelineNode::CreateComputeGraphNode() {
    if (gpuCtx == nullptr) {
        Logger() << "gpuCtx is null" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (computeElements.empty()) {
        Logger() << "no compute element" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    const std::string computeShaderPath = shaderPath;
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    const auto [pushConstantInfo, buffers, customDrawFunc] = computeElements[0];
    for (uint32_t i = 0; i < buffers.size(); ++i) {
        VkDescriptorSetLayoutBinding bufferBinding;
        bufferBinding.binding = i;
        if (buffers[i].type == PIPELINE_NODE_BUFFER_UNIFORM) {
            bufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        } else if (buffers[i].type == PIPELINE_NODE_BUFFER_STORAGE_READ |
                   buffers[i].type == PIPELINE_NODE_BUFFER_STORAGE_WRITE) {
            bufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }
        bufferBinding.descriptorCount = 1;
        bufferBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        bufferBinding.pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings.push_back(bufferBinding);
    }

    std::vector<VkPushConstantRange> pushConstantRanges;
    VkPushConstantRange pushConstantRange;
    pushConstantRange.offset = 0;
    pushConstantRange.size = pushConstantInfo.size;
    pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pushConstantRanges.push_back(pushConstantRange);

    computePipeline = std::make_shared<VkGPUComputePipeline>(computeShaderPath,
                                                             descriptorSetLayoutBindings,
                                                             pushConstantRanges);
    VkResult ret = computePipeline->CreateComputePipeline(gpuCtx->GetCurrentDevice(),
                                                          gpuCtx->GetPipelineCache());
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute pipeline, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    for (const auto &[pushConstantInfo, buffers, customDrawFunc]: computeElements) {
        auto descriptorSet = std::make_shared<VkGPUDescriptorSet>(
            gpuCtx->GetCurrentDevice(),
            computePipeline->GetPipelineLayout(),
            computePipeline->GetDescriptorSetLayout());
        ret = descriptorSet->AllocateDescriptorSets(gpuCtx->GetDescriptorPool());
        if (ret != VK_SUCCESS) {
            Logger() << "Failed to allocate descriptor sets, err =" << string_VkResult(ret) << std::endl;
            return ret;
        }

        for (const auto &buffer: buffers) {
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.offset = 0;
            bufferInfo.range = buffer.bufferSize;
            bufferInfo.buffer = buffer.buffer;
            this->pipelineDescriptorBufferInfos.push_back(bufferInfo);
        }
        for (uint32_t i = 0; i < pipelineDescriptorBufferInfos.size(); ++i) {
            descriptorSet->AddStorageBufferDescriptorSet(i, pipelineDescriptorBufferInfos.at(i));
        }
        descriptorSet->UpdateDescriptorSets();
        pipelineDescriptorSets.push_back(descriptorSet);
    }
    return ret;
}

void ComputePipelineNode::Compute(const VkCommandBuffer commandBuffer) {
    if (!this->dependencies.empty()) {
        for (const auto &dependence: this->dependencies) {
            Logger() << "Node: " << name << " Depend On:" << dependence->GetName() << std::endl;
            dependence->Compute(commandBuffer);
        }
    }
    Logger() << "Executing Compute Node: " << name << std::endl;
    computePipeline->GPUCmdBindPipeline(commandBuffer);
    for (size_t i = 0; i < computeElements.size(); ++i) {
        pipelineDescriptorSets[i]->GPUCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE);
        VkGPUHelper::GPUCmdPushConstant(commandBuffer,
                                        computePipeline->GetPipelineLayout(),
                                        VK_SHADER_STAGE_COMPUTE_BIT,
                                        0,
                                        computeElements[i].pushConstantInfo.size,
                                        computeElements[i].pushConstantInfo.data);

        std::vector<VkBufferMemoryBarrier> readBufferMemoryBarriers;
        for (const auto &[type, bufferSize, buffer]: computeElements[i].buffers) {
            if (type == PIPELINE_NODE_BUFFER_STORAGE_READ) {
                readBufferMemoryBarriers.push_back(VkGPUHelper::BuildBufferMemoryBarrier(
                    VK_ACCESS_MEMORY_WRITE_BIT,
                    VK_ACCESS_MEMORY_READ_BIT,
                    buffer,
                    bufferSize));
            }
        }

        PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2Fn = vkCmdPipelineBarrier2Fn = VkGPUHelper::GetVkCmdPipelineBarrier2Fn(this->gpuCtx->GetCurrentDevice());
        if (vkCmdPipelineBarrier2Fn != nullptr) {
            VkMemoryBarrier2KHR memoryBarrier;
            memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR;
            memoryBarrier.pNext = nullptr;
            memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR;
            memoryBarrier.srcAccessMask = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT_KHR;
            memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR;
            memoryBarrier.dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;

            VkDependencyInfoKHR dependencyInfo;
            dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
            dependencyInfo.pNext = nullptr;
            dependencyInfo.dependencyFlags = 0;
            dependencyInfo.memoryBarrierCount = 1;
            dependencyInfo.pMemoryBarriers = &memoryBarrier;
            dependencyInfo.bufferMemoryBarrierCount = 0;
            dependencyInfo.pBufferMemoryBarriers = nullptr;
            dependencyInfo.imageMemoryBarrierCount = 0;
            dependencyInfo.pImageMemoryBarriers = nullptr;
            vkCmdPipelineBarrier2Fn(commandBuffer, &dependencyInfo);
        }

        VkGPUHelper::GPUCmdPipelineBufferMemBarrier(commandBuffer,
                                                    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                    0,
                                                    readBufferMemoryBarriers);

        VkGPUHelper::GPUCmdDispatch(commandBuffer, this->workGroupCountX, workGroupCountY, workGroupCountZ);
        std::vector<VkBufferMemoryBarrier> writeBufferMemoryBarriers;
        for (const auto &[type, bufferSize, buffer]: computeElements[i].buffers) {
            if (type == PIPELINE_NODE_BUFFER_STORAGE_WRITE) {
                writeBufferMemoryBarriers.push_back(VkGPUHelper::BuildBufferMemoryBarrier(
                    VK_ACCESS_MEMORY_WRITE_BIT,
                    VK_ACCESS_MEMORY_READ_BIT,
                    buffer,
                    bufferSize));
            }
        }

        VkGPUHelper::GPUCmdPipelineBufferMemBarrier(commandBuffer,
                                                    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                    0,
                                                    writeBufferMemoryBarriers);
        std::vector<VkMemoryBarrier> memoryBarriers;
        memoryBarriers.push_back(VkGPUHelper::BuildMemoryBarrier(VK_ACCESS_MEMORY_WRITE_BIT,
            VK_ACCESS_MEMORY_READ_BIT));
        VkGPUHelper::GPUCmdPipelineMemBarrier(commandBuffer,
                                                   VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                   VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                   0,
                                                   memoryBarriers);

        if (computeElements[i].customDrawFunc != nullptr) {
            computeElements[i].customDrawFunc(commandBuffer);
        }
    }
}

void ComputePipelineNode::Destroy() {
    computePipeline->Destroy();
    for (const auto &pipelineDescriptorSet: pipelineDescriptorSets) {
        pipelineDescriptorSet->Destroy();
    }
}
