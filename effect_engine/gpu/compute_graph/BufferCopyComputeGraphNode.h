//
// Created by neo on 2025/3/11.
//

#ifndef BUFFERCOPYCOMPUTEGRAPHNODE_H
#define BUFFERCOPYCOMPUTEGRAPHNODE_H

#include "IComputeGraphNode.h"
#include "effect_engine/gpu/VkGPUContext.h"

typedef struct {
    VkDeviceSize bufferSize;
    VkBuffer buffer;
} BufferCopyNodeBufferInfo;

class BufferCopyComputeGraphNode final : public IComputeGraphNode {
    BufferCopyNodeBufferInfo srcBuffer{};
    BufferCopyNodeBufferInfo dstBuffer{};
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;

public:
    BufferCopyComputeGraphNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                               const std::string &name,
                               BufferCopyNodeBufferInfo srcBuffer,
                               BufferCopyNodeBufferInfo dstBuffer);

    ~BufferCopyComputeGraphNode() override = default;

    VkResult CreateComputeGraphNode() override;

    void Compute(VkCommandBuffer commandBuffer) override;

    void Destroy() override;
};


#endif //BUFFERCOPYCOMPUTEGRAPHNODE_H
