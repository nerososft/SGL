//
// Created by neo on 25-7-4.
//

#ifndef ROPEMULOPERATOR_H
#define ROPEMULOPERATOR_H
#include <cstdint>

#include "engine/compute/operators/BinaryOperator.h"
#include "engine/compute/operators/IOperator.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/VkGPUContext.h"

struct RoPEMulOperatorParams {
  uint32_t ropeTheta;
  uint32_t m;
  uint32_t n;
};

class RoPEMulOperator final : public BinaryOperator {
  RoPEMulOperatorParams params{};

public:
  RoPEMulOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                  const std::shared_ptr<VkGPUBuffer> &Q,
                  const std::shared_ptr<VkGPUBuffer> &K,
                  const std::shared_ptr<VkGPUBuffer> &output);

  ~RoPEMulOperator() override;

  void SetRopeTheta(const uint32_t ropeTheta) {
    this->params.ropeTheta = ropeTheta;
  }

  void SetM(const uint32_t m) { this->params.m = m; }

  void SetN(const uint32_t n) { this->params.n = n; }

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void Destroy() override;
};

#endif // ROPEMULOPERATOR_H
