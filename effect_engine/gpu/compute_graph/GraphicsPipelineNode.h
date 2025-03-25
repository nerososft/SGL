//
// Created by neo on 2025/3/25.
//

#ifndef GRAPHICSPIPELINENODE_H
#define GRAPHICSPIPELINENODE_H
#include "IComputeGraphNode.h"
#include "effect_engine/gpu/VkGPUComputePipeline.h"
#include "effect_engine/gpu/VkGPUContext.h"
#include "effect_engine/gpu/VkGPUDescriptorSet.h"

class GraphicsPipelineNode final : public IComputeGraphNode {
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    PushConstantInfo pushConstantInfo{};

    std::vector<PipelineNodeBuffer> pipelineBuffers;

    uint32_t width = 1;
    uint32_t height = 1;

    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<VkGPUComputePipeline> graphicsPipeline = nullptr;
    std::vector<VkDescriptorBufferInfo> pipelineDescriptorBufferInfos;
    std::shared_ptr<VkGPUDescriptorSet> pipelineDescriptorSet = nullptr;

public:
    GraphicsPipelineNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                         const std::string &name,
                         const std::string &vertexShaderPath,
                         const std::string &fragmentShaderPath,
                         PushConstantInfo pushConstantInfo,
                         const std::vector<PipelineNodeBuffer> &buffers,
                         uint32_t width,
                         uint32_t height);

    ~GraphicsPipelineNode() override = default;

    VkResult CreateComputeGraphNode() override;

    void Compute(VkCommandBuffer commandBuffer) override;

    void Destroy() override;
};


#endif //GRAPHICSPIPELINENODE_H
