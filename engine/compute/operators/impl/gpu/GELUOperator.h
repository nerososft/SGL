//
// Created by neo on 25-5-25.
//

#ifndef GELUOPERATOR_H
#define GELUOPERATOR_H

#include "engine/compute/operators/UnaryOperator.h"

class GELUOperator final : public UnaryOperator {
public:
  GELUOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
               const std::shared_ptr<VkGPUBuffer> &inputBuffer,
               const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  ~GELUOperator() override;

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void Destroy() override;
};

#endif // GELUOPERATOR_H
