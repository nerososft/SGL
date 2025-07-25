//
// Created by neo on 25-6-28.
//

#ifndef CONCATOPERATOR_H
#define CONCATOPERATOR_H
#include <memory>

#include "engine/compute/operators/Operator.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/VkGPUContext.h"
#include "runtime/gpu/compute_graph/IComputeGraphNode.h"

struct ConcatOperatorParams {
  uint32_t nums;
  uint32_t blockWidth;
  uint32_t blockHeight;
  uint32_t width;
  uint32_t height;
};

class ConcatOperator final : public Operator {
  ConcatOperatorParams params{};
  std::vector<std::shared_ptr<VkGPUBuffer>> inputBuffers;
  std::shared_ptr<VkGPUBuffer> outBuffer = nullptr;

public:
  ConcatOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<std::shared_ptr<VkGPUBuffer>> &inputBuffers,
                 const std::shared_ptr<VkGPUBuffer> &outBuffer);

  ~ConcatOperator() override;

  void SetNums(const size_t nums) { this->params.nums = nums; }

  void SetBlockWidth(const size_t width) { this->params.blockWidth = width; }

  void SetBlockHeight(const size_t height) {
    this->params.blockHeight = height;
  }

  void SetWidth(const size_t width) { this->params.width = width; }

  void SetHeight(const size_t height) { this->params.height = height; }

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void Destroy() override;
};

#endif // CONCATOPERATOR_H
