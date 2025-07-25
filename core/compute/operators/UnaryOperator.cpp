//
// Created by neo on 2025/3/29.
//

#include "UnaryOperator.h"

UnaryOperator::UnaryOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                             const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                             const std::shared_ptr<VkGPUBuffer> &outputBuffer) {
  this->gpuCtx = gpuCtx;
  this->inputBuffer = inputBuffer;
  this->outputBuffer = outputBuffer;
}

UnaryOperator::~UnaryOperator() { UnaryOperator::Destroy(); }

void UnaryOperator::Destroy() {}
