//
// Created by neo on 2025/3/25.
//

#include "GraphicsPipelineNode.h"

#include <vulkan/vk_enum_string_helper.h>

#include "effect_engine/gpu/VkGPUGraphicsPipeline.h"
#include "effect_engine/log/Log.h"

GraphicsPipelineNode::GraphicsPipelineNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                           const std::string &name,
                                           const std::shared_ptr<VkGPURenderPass> &renderPass,
                                           const std::string &vertexShaderPath,
                                           const std::string &fragmentShaderPath,
                                           const PushConstantInfo pushConstantInfo,
                                           const std::vector<PipelineNodeBuffer> &buffers,
                                           const float width,
                                           const float height) {
    this->gpuCtx = gpuCtx;
    this->name = name;
    this->renderPass = renderPass;
    this->vertexShaderPath = vertexShaderPath;
    this->fragmentShaderPath = fragmentShaderPath;
    this->pushConstantInfo = pushConstantInfo;
    this->width = width;
    this->height = height;
    this->pipelineBuffers = buffers;
}

VkResult GraphicsPipelineNode::CreateComputeGraphNode() {
    if (gpuCtx == nullptr) {
        Logger() << "gpuCtx is null" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
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
        bufferBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
        bufferBinding.pImmutableSamplers = nullptr;
        descriptorSetLayoutBindings.push_back(bufferBinding);
    }

    std::vector<VkPushConstantRange> pushConstantRanges;
    VkPushConstantRange pushConstantRange;
    pushConstantRange.offset = 0;
    pushConstantRange.size = pushConstantInfo.size;
    pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
    pushConstantRanges.push_back(pushConstantRange);

    // TODO:
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;

    graphicsPipeline = std::make_shared<VkGPUGraphicsPipeline>(vertexShaderPath,
                                                               fragmentShaderPath,
                                                               width,
                                                               height,
                                                               descriptorSetLayoutBindings,
                                                               pushConstantRanges,
                                                               vertexInputBindingDescriptions,
                                                               vertexInputAttributeDescriptions);
    VkResult ret = graphicsPipeline->CreateGraphicsPipeline(gpuCtx->GetCurrentDevice(),
                                                            gpuCtx->GetPipelineCache(),
                                                            renderPass->GetRenderPass());
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute pipeline, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    pipelineDescriptorSet = std::make_shared<VkGPUDescriptorSet>(gpuCtx->GetCurrentDevice(),
                                                                 graphicsPipeline->GetPipelineLayout(),
                                                                 graphicsPipeline->GetDescriptorSetLayout());
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

void GraphicsPipelineNode::Compute(VkCommandBuffer commandBuffer) {
    // TODO:
}

void GraphicsPipelineNode::Destroy() {
}
