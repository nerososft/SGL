//
// Created by neo on 25-7-21.
//

#ifndef SINOPERATOR_H
#define SINOPERATOR_H
#include "engine/compute/operators/UnaryOperator.h"

class SinOperator final : public UnaryOperator {
public:
  SinOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
              const std::shared_ptr<VkGPUBuffer> &inputBuffer,
              const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  ~SinOperator() override;

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void Destroy() override;
};

#endif // SINOPERATOR_H
