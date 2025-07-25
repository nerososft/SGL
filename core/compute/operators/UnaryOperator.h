//
// Created by neo on 2025/3/29.
//

#ifndef UNARYOPERATOR_H
#define UNARYOPERATOR_H
#include "IOperator.h"
#include "runtime/gpu/VkGPUBuffer.h"

class UnaryOperator : public IOperator {
public:
  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
  std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> outputBuffer = nullptr;

  UnaryOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  ~UnaryOperator() override;

  void Destroy() override;
};

#endif // UNARYOPERATOR_H
