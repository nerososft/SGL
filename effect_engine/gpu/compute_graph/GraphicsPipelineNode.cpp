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
                                           const float width,
                                           const float height) {
    this->gpuCtx = gpuCtx;
    this->name = name;
    this->renderPass = renderPass;
    this->vertexShaderPath = vertexShaderPath;
    this->fragmentShaderPath = fragmentShaderPath;
    this->width = width;
    this->height = height;
}

void GraphicsPipelineNode::AddGraphicsElement(const GraphicsElement &graphicsElement) {
    this->graphicsElements.push_back(graphicsElement);
}

VkResult GraphicsPipelineNode::CreateComputeGraphNode() {
    if (gpuCtx == nullptr) {
        Logger() << "gpuCtx is null" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (graphicsElements.empty()) {
        Logger() << "no graphics element" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    const auto [pushConstantInfo, buffers] = graphicsElements[0];
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


    for (const auto &[pushConstantInfo, buffers]: graphicsElements) {
        auto descriptorSet = std::make_shared<VkGPUDescriptorSet>(
            gpuCtx->GetCurrentDevice(),
            graphicsPipeline->GetPipelineLayout(),
            graphicsPipeline->GetDescriptorSetLayout());
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

void GraphicsPipelineNode::Compute(VkCommandBuffer commandBuffer) {
    // TODO:
}

void GraphicsPipelineNode::Destroy() {
}
