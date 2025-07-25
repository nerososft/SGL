//
// Created by neo on 25-6-30.
//

#ifndef ADDOPERATOR_H
#define ADDOPERATOR_H
#include "core/compute/operators/BinaryOperator.h"

class AddOperator final : public BinaryOperator {
public:
  AddOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
              const std::shared_ptr<VkGPUBuffer> &inputBuffer1,
              const std::shared_ptr<VkGPUBuffer> &inputBuffer2,
              const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  ~AddOperator() override;

  void Destroy() override;
};

#endif // ADDOPERATOR_H
