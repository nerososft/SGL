//
// Created by neo on 2025/3/29.
//

#include "BinaryOperator.h"

BinaryOperator::BinaryOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer1,
                               const std::shared_ptr<VkGPUBuffer> &inputBuffer2,
                               const std::shared_ptr<VkGPUBuffer> &outputBuffer) {
    this->inputBuffer1 = inputBuffer1;
    this->inputBuffer2 = inputBuffer2;
    this->outputBuffer = outputBuffer;
}

BinaryOperator::~BinaryOperator() {
    this->Destroy();
}

void BinaryOperator::Destroy() const {
    if (outputBuffer != nullptr) {
        outputBuffer->Destroy();
    }
    if (inputBuffer1 != nullptr) {
        inputBuffer1->Destroy();
    }
    if (inputBuffer2 != nullptr) {
        inputBuffer2->Destroy();
    }
}
