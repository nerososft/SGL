//
// Created by neo on 2025/3/11.
//

#ifndef BUFFERCOPYCOMPUTEGRAPHNODE_H
#define BUFFERCOPYCOMPUTEGRAPHNODE_H

#include "ComputePipelineNode.h"
#include "IComputeGraphNode.h"
#include "core/gpu/VkGPUContext.h"

typedef struct {
  VkDeviceSize bufferSize;
  VkBuffer buffer;
} BufferCopyNodeBufferInfo;

class BufferCopyNode final : public IComputeGraphNode {
  BufferCopyNodeBufferInfo srcBuffer{};
  BufferCopyNodeBufferInfo dstBuffer{};
  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;

public:
  BufferCopyNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::string &name, BufferCopyNodeBufferInfo srcBuffer,
                 BufferCopyNodeBufferInfo dstBuffer);

  ~BufferCopyNode() override = default;

  VkResult CreateComputeGraphNode() override;

  void Compute(VkCommandBuffer commandBuffer) override;

  void Destroy() override;
};

#endif // BUFFERCOPYCOMPUTEGRAPHNODE_H
