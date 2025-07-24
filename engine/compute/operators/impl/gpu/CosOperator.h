//
// Created by neo on 25-7-21.
//

#ifndef COSOPERATOR_H
#define COSOPERATOR_H
#include "engine/compute/operators/UnaryOperator.h"

class CosOperator final : public UnaryOperator {
public:
  CosOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
              const std::shared_ptr<VkGPUBuffer> &inputBuffer,
              const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  ~CosOperator() override;

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void Destroy() override;
};

#endif // COSOPERATOR_H
