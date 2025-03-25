//
// Created by neo on 2025/3/25.
//

#include "GraphicsPipelineNode.h"

GraphicsPipelineNode::GraphicsPipelineNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                           const std::string &name,
                                           const std::string &vertexShaderPath,
                                           const std::string &fragmentShaderPath,
                                           const PushConstantInfo pushConstantInfo,
                                           const std::vector<PipelineNodeBuffer> &buffers,
                                           const uint32_t width,
                                           const uint32_t height) {
    this->gpuCtx = gpuCtx;
    this->name = name;
    this->vertexShaderPath = vertexShaderPath;
    this->fragmentShaderPath = fragmentShaderPath;
    this->pushConstantInfo = pushConstantInfo;
    this->width = width;
    this->height = height;
    this->pipelineBuffers = buffers;
}

VkResult GraphicsPipelineNode::CreateComputeGraphNode() {
    VkResult result = VK_SUCCESS;
    // TODO:
    return result;
}

void GraphicsPipelineNode::Compute(VkCommandBuffer commandBuffer) {
    // TODO:
}

void GraphicsPipelineNode::Destroy() {
}
