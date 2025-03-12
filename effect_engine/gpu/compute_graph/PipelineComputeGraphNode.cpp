//
// Created by neo on 2025/3/11.
//

#include "PipelineComputeGraphNode.h"

#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

#include "effect_engine/gpu/VkGPUComputePipeline.h"
#include "effect_engine/gpu/VkGPUDescriptorSet.h"
#include "effect_engine/gpu/VkGPUHelper.h"

PipelineComputeGraphNode::PipelineComputeGraphNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                                   const std::string &name,
                                                   const std::string &shaderPath,
                                                   const PushConstantInfo pushConstantInfo,
                                                   const PipelineNodeInput input,
                                                   const PipelineNodeOutput output,
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
    this->input = input;
    this->output = output;
}

VkResult PipelineComputeGraphNode::CreateComputeGraphNode() {
    if (gpuCtx == nullptr) {
        std::cerr << "gpuCtx is null" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    const std::string computeShaderPath = shaderPath;
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
    pushConstantRange.size = pushConstantInfo.size;
    pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pushConstantRanges.push_back(pushConstantRange);

    computePipeline = std::make_shared<VkGPUComputePipeline>(computeShaderPath,
                                                             descriptorSetLayoutBindings,
                                                             pushConstantRanges);
    VkResult ret = computePipeline->CreateComputePipeline(gpuCtx->GetCurrentDevice(),
                                                          gpuCtx->GetPipelineCache());
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to create compute pipeline, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    pipelineDescriptorSet = std::make_shared<VkGPUDescriptorSet>(gpuCtx->GetCurrentDevice(),
                                                                 computePipeline->GetPipelineLayout(),
                                                                 computePipeline->GetDescriptorSetLayout());
    ret = pipelineDescriptorSet->AllocateDescriptorSets(gpuCtx->GetDescriptorPool());
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to allocate descriptor sets, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }
    VkDescriptorBufferInfo inputImageBufferInfo = {};
    inputImageBufferInfo.offset = 0;
    inputImageBufferInfo.range = input.bufferSize;
    inputImageBufferInfo.buffer = input.buffer;
    pipelineDescriptorSet->AddStorageBufferDescriptorSet(0, inputImageBufferInfo);

    VkDescriptorBufferInfo outputImageBufferInfo = {};
    outputImageBufferInfo.offset = 0;
    outputImageBufferInfo.range = output.bufferSize;
    outputImageBufferInfo.buffer = output.buffer;
    pipelineDescriptorSet->AddStorageBufferDescriptorSet(1, outputImageBufferInfo);

    pipelineDescriptorSet->UpdateDescriptorSets();
    return ret;
}

void PipelineComputeGraphNode::Compute(const VkCommandBuffer commandBuffer) {
    std::cout << "Executing Compute Node: " << name << std::endl;
    if (!this->dependencies.empty()) {
        for (const auto &dependence: this->dependencies) {
            std::cout << "Node: " << name << " Depend On:" << dependence->GetName() << std::endl;
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
    bufferMemoryBarriers.push_back(VkGPUHelper::BuildBufferMemoryBarrier(VK_ACCESS_SHADER_WRITE_BIT,
                                                                         VK_ACCESS_TRANSFER_READ_BIT,
                                                                         output.buffer,
                                                                         output.bufferSize));
    VkGPUHelper::GPUCmdPipelineBufferMemBarrier(commandBuffer,
                                                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                0,
                                                bufferMemoryBarriers);
}

void PipelineComputeGraphNode::Destroy() {
}
