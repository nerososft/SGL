//
// Created by neo on 2025/3/25.
//

#include "GraphicsPipelineNode.h"

#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/gpu/VkGPUGraphicsPipeline.h"
#include "effect_engine/gpu/VkGPUHelper.h"
#include "effect_engine/log/Log.h"

GraphicsPipelineNode::GraphicsPipelineNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                           const std::string &name,
                                           const std::shared_ptr<VkGPURenderPass> &renderPass,
                                           const std::string &vertexShaderPath,
                                           const std::string &fragmentShaderPath,
                                           const std::vector<VkVertexInputBindingDescription> &
                                           vertexInputBindingDescriptions,
                                           const std::vector<VkVertexInputAttributeDescription> &
                                           vertexInputAttributeDescriptions,
                                           const float width,
                                           const float height) {
    this->gpuCtx = gpuCtx;
    this->name = name;
    this->type = COMPUTE_GRAPH_NODE_GRAPHICS;
    this->renderPass = renderPass;
    this->vertexShaderPath = vertexShaderPath;
    this->fragmentShaderPath = fragmentShaderPath;
    this->vertexInputBindingDescriptions = vertexInputBindingDescriptions;
    this->vertexInputAttributeDescriptions = vertexInputAttributeDescriptions;
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
    const auto [pushConstantInfo, buffers, customDrawFunc] = graphicsElements[0];
    for (uint32_t i = 0; i < buffers.size(); ++i) {
        if (buffers[i].type == PIPELINE_NODE_BUFFER_VERTEX || buffers[i].type == PIPELINE_NODE_BUFFER_INDEX) {
            break;
        }
        VkDescriptorSetLayoutBinding bufferBinding;
        bufferBinding.binding = i;
        if (buffers[i].type == PIPELINE_NODE_BUFFER_UNIFORM) {
            bufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        } else if (buffers[i].type == PIPELINE_NODE_BUFFER_STORAGE_READ ||
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


    for (const auto &[pushConstantInfo, buffers, customDrawFunc]: graphicsElements) {
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
            if (buffer.type == PIPELINE_NODE_BUFFER_VERTEX || buffer.type == PIPELINE_NODE_BUFFER_INDEX) {
                break;
            }
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

void GraphicsPipelineNode::Compute(const VkCommandBuffer commandBuffer) {
    if (!this->dependencies.empty()) {
        for (const auto &dependence: this->dependencies) {
            Logger() << "Node: " << name << " Depend On:" << dependence->GetName() << std::endl;
            dependence->Compute(commandBuffer);
        }
    }
    Logger() << "Executing Compute Node: " << name << std::endl;
    graphicsPipeline->GPUCmdBindPipeline(commandBuffer);
    for (size_t i = 0; i < graphicsElements.size(); ++i) {
        pipelineDescriptorSets[i]->GPUCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE);
        VkGPUHelper::GPUCmdPushConstant(commandBuffer,
                                        graphicsPipeline->GetPipelineLayout(),
                                        VK_SHADER_STAGE_ALL_GRAPHICS,
                                        0,
                                        graphicsElements[i].pushConstantInfo.size,
                                        graphicsElements[i].pushConstantInfo.data);

        std::vector<VkBuffer> bindVertexBuffers;
        std::vector<VkDeviceSize> bindVertexOffsets;
        std::vector<VkBuffer> bindIndexBuffers;
        for (const auto &buffer: graphicsElements[i].buffers) {
            if (buffer.type == PIPELINE_NODE_BUFFER_VERTEX) {
                bindVertexBuffers.push_back(buffer.buffer);
                bindVertexOffsets.push_back(buffer.bufferSize);
            }
            if (buffer.type == PIPELINE_NODE_BUFFER_INDEX) {
                bindIndexBuffers.push_back(buffer.buffer);
            }
        }
        if (!bindVertexBuffers.empty()) {
            vkCmdBindVertexBuffers(commandBuffer,
                                   0,
                                   bindVertexBuffers.size(),
                                   bindVertexBuffers.data(),
                                   bindVertexOffsets.data());
        }
        for (const auto &buffer: bindIndexBuffers) {
            vkCmdBindIndexBuffer(commandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
        }
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        if (graphicsElements[i].customDrawFunc != nullptr) {
            graphicsElements[i].customDrawFunc(commandBuffer);
        }
    }
}

void GraphicsPipelineNode::Destroy() {
    if (graphicsPipeline != nullptr) {
        graphicsPipeline->Destroy();
        graphicsPipeline = nullptr;
    }
    for (const auto &pipelineDescriptorSet: pipelineDescriptorSets) {
        pipelineDescriptorSet->Destroy();
    }
    pipelineDescriptorSets.clear();
    IComputeGraphNode::Destroy();
}
