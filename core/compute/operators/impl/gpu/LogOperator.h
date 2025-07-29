//
// Created by neo on 25-7-29.
//

#ifndef LOGOPERATOR_H
#define LOGOPERATOR_H
#include "core/compute/operators/UnaryOperator.h"
#include "runtime/gpu/VkGPUContext.h"
#include "runtime/gpu/compute_graph/IComputeGraphNode.h"

#include <memory>

class LogOperator final : public UnaryOperator {
public:
  LogOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
              const std::shared_ptr<VkGPUBuffer> &inputBuffer,
              const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  ~LogOperator() override;

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void Destroy() override;
};

#endif // LOGOPERATOR_H
