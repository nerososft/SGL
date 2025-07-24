//
// Created by neo on 25-5-25.
//

#ifndef SOFTMAXOPERATOR_H
#define SOFTMAXOPERATOR_H

#include "engine/compute/operators/UnaryOperator.h"

struct SoftmaxOperatorParams {
  float sum;
  float max;
};

class SoftmaxOperator final : public UnaryOperator {
  SoftmaxOperatorParams params{};

  float *sum = nullptr;
  float *max = nullptr;

public:
  SoftmaxOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                  const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                  const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  ~SoftmaxOperator() override;

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void SetSum(float *sum) { this->sum = sum; }

  void SetMax(float *max) { this->max = max; }

  void Destroy() override;
};

#endif // SOFTMAXOPERATOR_H
