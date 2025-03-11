//
// Created by neo on 2025/3/11.
//

#ifndef PIPELINECOMPUTEGRAPHNODE_H
#define PIPELINECOMPUTEGRAPHNODE_H

#include "IComputeGraphNode.h"
#include "effect_engine/gpu/VkGPUComputePipeline.h"
#include "effect_engine/gpu/VkGPUContext.h"
#include "effect_engine/gpu/VkGPUDescriptorSet.h"

typedef struct {
    VkDeviceSize size;
    void *data;
} PushConstantInfo;

typedef struct {
    VkDeviceSize bufferSize;
    VkBuffer buffer;
} PipelineNodeInput;

typedef PipelineNodeInput PipelineNodeOutput;

class PipelineComputeGraphNode final : public IComputeGraphNode {
    std::string shaderPath;
    PushConstantInfo pushConstantInfo{};

    PipelineNodeInput input{};
    PipelineNodeOutput output{};

    uint32_t workGroupCountX = 1;
    uint32_t workGroupCountY = 1;
    uint32_t workGroupCountZ = 1;

    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<VkGPUComputePipeline> computePipeline = nullptr;
    std::shared_ptr<VkGPUDescriptorSet> pipelineDescriptorSet = nullptr;

public:
    PipelineComputeGraphNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                             const std::string &name,
                             const std::string &shaderPath,
                             PushConstantInfo pushConstantInfo,
                             PipelineNodeInput input,
                             PipelineNodeOutput output,
                             uint32_t workGroupCountX,
                             uint32_t workGroupCountY,
                             uint32_t workGroupCountZ);

    ~PipelineComputeGraphNode() override = default;

    VkResult CreateComputeGraphNode() override;

    void Compute(VkCommandBuffer commandBuffer) override;
};


#endif //PIPELINECOMPUTEGRAPHNODE_H
