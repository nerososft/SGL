//
// Created by neo on 2025/3/11.
//

#include "PipelineComputeGraphNode.h"

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

PipelineComputeGraphNode::PipelineComputeGraphNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                                   const std::string &name,
                                                   const std::string &shaderPath,
                                                   const PushConstantInfo pushConstantInfo,
                                                   const std::vector<PipelineNodeBuffer> &buffers,
                                                   const uint32_t workGroupCountX,
                                                   const uint32_t workGroupCountY,
                                                   const uint32_t workGroupCountZ) {
    this->gpuCtx = gpuCtx;
    this->name = name;
    this->shaderPath = shaderPath;
    this->pushConstantInfo = pushConstantInfo;
    this->workGroupCountX = workGroupCountX;
    this->workGroupCountY = workGroupCountY;
    this->workGroupCountZ = workGroupCountZ;
    this->pipelineBuffers = buffers;
}

VkResult PipelineComputeGraphNode::CreateComputeGraphNode() {
    if (gpuCtx == nullptr) {
        std::cerr << "gpuCtx is null" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    const std::string computeShaderPath = shaderPath;
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

    for (uint32_t i = 0; i < pipelineBuffers.size(); ++i) {
        VkDescriptorSetLayoutBinding bufferBinding;
        bufferBinding.binding = i;
        if (pipelineBuffers[i].type == PIPELINE_NODE_BUFFER_UNIFORM) {
            bufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        } else if (pipelineBuffers[i].type == PIPELINE_NODE_BUFFER_STORAGE_READ |
                   pipelineBuffers[i].type == PIPELINE_NODE_BUFFER_STORAGE_WRITE) {
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

    pipelineDescriptorSet = std::make_shared<VkGPUDescriptorSet>(gpuCtx->GetCurrentDevice(),
                                                                 computePipeline->GetPipelineLayout(),
                                                                 computePipeline->GetDescriptorSetLayout());
    ret = pipelineDescriptorSet->AllocateDescriptorSets(gpuCtx->GetDescriptorPool());
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to allocate descriptor sets, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    for (const auto &pipelineBuffer: pipelineBuffers) {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.offset = 0;
        bufferInfo.range = pipelineBuffer.bufferSize;
        bufferInfo.buffer = pipelineBuffer.buffer;
        this->pipelineDescriptorBufferInfos.push_back(bufferInfo);
    }
    for (uint32_t i = 0; i < pipelineDescriptorBufferInfos.size(); ++i) {
        pipelineDescriptorSet->AddStorageBufferDescriptorSet(i, pipelineDescriptorBufferInfos.at(i));
    }
    pipelineDescriptorSet->UpdateDescriptorSets();
    return ret;
}

void PipelineComputeGraphNode::Compute(const VkCommandBuffer commandBuffer) {
    Logger() << "Executing Compute Node: " << name << std::endl;
    if (!this->dependencies.empty()) {
        for (const auto &dependence: this->dependencies) {
            Logger() << "Node: " << name << " Depend On:" << dependence->GetName() << std::endl;
            dependence->Compute(commandBuffer);
        }
    }
    computePipeline->GPUCmdBindPipeline(commandBuffer);
    pipelineDescriptorSet->GPUCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE);
    VkGPUHelper::GPUCmdPushConstant(commandBuffer,
                                    computePipeline->GetPipelineLayout(),
                                    VK_SHADER_STAGE_COMPUTE_BIT,
                                    0,
                                    pushConstantInfo.size,
                                    pushConstantInfo.data);
    VkGPUHelper::GPUCmdDispatch(commandBuffer, this->workGroupCountX, workGroupCountY, workGroupCountZ);

    std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers;
    for (const auto &pipelineBuffer: pipelineBuffers) {
        if (pipelineBuffer.type == PIPELINE_NODE_BUFFER_STORAGE_WRITE) {
            bufferMemoryBarriers.push_back(VkGPUHelper::BuildBufferMemoryBarrier(VK_ACCESS_SHADER_WRITE_BIT,
                VK_ACCESS_TRANSFER_READ_BIT,
                pipelineBuffer.buffer,
                pipelineBuffer.bufferSize));
        }
    }

    VkGPUHelper::GPUCmdPipelineBufferMemBarrier(commandBuffer,
                                                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                0,
                                                bufferMemoryBarriers);
}

void PipelineComputeGraphNode::Destroy() {
}
