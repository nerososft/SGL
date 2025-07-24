//
// Created by neo on 25-5-25.
//

#ifndef SILUOPERATOR_H
#define SILUOPERATOR_H

#include "engine/compute/operators/UnaryOperator.h"

class SiLUOperator final : public UnaryOperator {
public:
  SiLUOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
               const std::shared_ptr<VkGPUBuffer> &inputBuffer,
               const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  ~SiLUOperator() override;

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void Destroy() override;
};

#endif // SILUOPERATOR_H
