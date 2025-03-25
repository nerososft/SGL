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

typedef enum {
    PIPELINE_NODE_BUFFER_UNIFORM,
    PIPELINE_NODE_BUFFER_STORAGE_READ,
    PIPELINE_NODE_BUFFER_STORAGE_WRITE,
} PipelineNodeBufferType;

typedef struct {
    PipelineNodeBufferType type;
    VkDeviceSize bufferSize;
    VkBuffer buffer;
} PipelineNodeBuffer;

class ComputePipelineNode final : public IComputeGraphNode {
    std::string shaderPath;
    PushConstantInfo pushConstantInfo{};

    std::vector<PipelineNodeBuffer> pipelineBuffers;

    uint32_t workGroupCountX = 1;
    uint32_t workGroupCountY = 1;
    uint32_t workGroupCountZ = 1;

    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<VkGPUComputePipeline> computePipeline = nullptr;
    std::vector<VkDescriptorBufferInfo> pipelineDescriptorBufferInfos;
    std::shared_ptr<VkGPUDescriptorSet> pipelineDescriptorSet = nullptr;

public:
    ComputePipelineNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                             const std::string &name,
                             const std::string &shaderPath,
                             PushConstantInfo pushConstantInfo,
                             const std::vector<PipelineNodeBuffer> &buffers,
                             uint32_t workGroupCountX,
                             uint32_t workGroupCountY,
                             uint32_t workGroupCountZ);

    ~ComputePipelineNode() override = default;

    VkResult CreateComputeGraphNode() override;

    void Compute(VkCommandBuffer commandBuffer) override;

    void Destroy() override;
};


#endif //PIPELINECOMPUTEGRAPHNODE_H
