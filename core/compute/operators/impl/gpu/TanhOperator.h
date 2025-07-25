//
// Created by neo on 25-5-25.
//

#ifndef TANHOPERATOR_H
#define TANHOPERATOR_H

#include "core/compute/operators/UnaryOperator.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/VkGPUContext.h"

class TanhOperator final : public UnaryOperator {
public:
  TanhOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
               const std::shared_ptr<VkGPUBuffer> &inputBuffer,
               const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  ~TanhOperator() override;

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void Destroy() override;
};

#endif // TANHOPERATOR_H
