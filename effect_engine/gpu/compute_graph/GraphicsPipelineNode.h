//
// Created by neo on 2025/3/25.
//

#ifndef GRAPHICSPIPELINENODE_H
#define GRAPHICSPIPELINENODE_H
#include "IComputeGraphNode.h"
#include "effect_engine/gpu/VkGPUComputePipeline.h"
#include "effect_engine/gpu/VkGPUContext.h"
#include "effect_engine/gpu/VkGPUDescriptorSet.h"
#include "effect_engine/gpu/VkGPUGraphicsPipeline.h"
#include "effect_engine/gpu/VkGPURenderPass.h"

class GraphicsPipelineNode final : public IComputeGraphNode {
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    PushConstantInfo pushConstantInfo{};

    std::vector<PipelineNodeBuffer> pipelineBuffers;

    float width = 1.0f;
    float height = 1.0f;

    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<VkGPURenderPass> renderPass = nullptr;
    std::shared_ptr<VkGPUGraphicsPipeline> graphicsPipeline = nullptr;
    std::vector<VkDescriptorBufferInfo> pipelineDescriptorBufferInfos;
    std::shared_ptr<VkGPUDescriptorSet> pipelineDescriptorSet = nullptr;

public:
    GraphicsPipelineNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                         const std::string &name,
                         const std::shared_ptr<VkGPURenderPass> &renderPass,
                         const std::string &vertexShaderPath,
                         const std::string &fragmentShaderPath,
                         PushConstantInfo pushConstantInfo,
                         const std::vector<PipelineNodeBuffer> &buffers,
                         float width,
                         float height);

    ~GraphicsPipelineNode() override = default;

    VkResult CreateComputeGraphNode() override;

    void Compute(VkCommandBuffer commandBuffer) override;

    void Destroy() override;
};


#endif //GRAPHICSPIPELINENODE_H
