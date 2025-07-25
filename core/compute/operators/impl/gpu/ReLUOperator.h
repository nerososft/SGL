//
// Created by neo on 25-5-7.
//

#ifndef RELUOPERATOR_H
#define RELUOPERATOR_H
#include "core/compute/operators/UnaryOperator.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/VkGPUContext.h"

class ReLUOperator final : public UnaryOperator {
public:
  ReLUOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
               const std::shared_ptr<VkGPUBuffer> &inputBuffer,
               const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  ~ReLUOperator() override;

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void Destroy() override;
};

#endif // RELUOPERATOR_H
