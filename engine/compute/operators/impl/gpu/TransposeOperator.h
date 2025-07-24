//
// Created by neo on 25-7-8.
//

#ifndef TRANSPOSEOPERATOR_H
#define TRANSPOSEOPERATOR_H

#include "engine/compute/operators/UnaryOperator.h"

struct TransposeOperatorParams {
  uint32_t width;
  uint32_t height;
};

class TransposeOperator final : public UnaryOperator {
  TransposeOperatorParams params{};

public:
  TransposeOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                    const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                    const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void SetMatSize(const uint32_t width, const uint32_t height) {
    this->params.width = width;
    this->params.height = height;
  }

  ~TransposeOperator() override;

  void Destroy() override;
};

#endif // TRANSPOSEOPERATOR_H
