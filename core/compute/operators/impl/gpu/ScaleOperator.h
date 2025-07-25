//
// Created by neo on 25-6-30.
//

#ifndef SCALEOPERATOR_H
#define SCALEOPERATOR_H
#include <memory>

#include "core/compute/operators/UnaryOperator.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/VkGPUContext.h"
#include "runtime/gpu/compute_graph/IComputeGraphNode.h"

struct ScaleOperatorParams {
  float delta;
};

class ScaleOperator final : public UnaryOperator {
  ScaleOperatorParams params{};

public:
  ScaleOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  ~ScaleOperator() override;

  void SetDelta(const float delta) { this->params.delta = delta; };

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void Destroy() override;
};

#endif // SCALEOPERATOR_H
