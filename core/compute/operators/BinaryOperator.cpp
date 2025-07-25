//
// Created by neo on 2025/3/29.
//

#include "BinaryOperator.h"

BinaryOperator::BinaryOperator(
    const std::shared_ptr<VkGPUContext> &gpuCtx,
    const std::shared_ptr<VkGPUBuffer> &inputBuffer1,
    const std::shared_ptr<VkGPUBuffer> &inputBuffer2,
    const std::shared_ptr<VkGPUBuffer> &outputBuffer) {
  this->gpuCtx = gpuCtx;
  this->inputBuffer1 = inputBuffer1;
  this->inputBuffer2 = inputBuffer2;
  this->outputBuffer = outputBuffer;
}

BinaryOperator::~BinaryOperator() { this->BinaryOperator::Destroy(); }

void BinaryOperator::Destroy() {}
