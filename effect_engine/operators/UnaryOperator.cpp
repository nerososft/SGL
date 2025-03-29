//
// Created by neo on 2025/3/29.
//

#include "UnaryOperator.h"

UnaryOperator::UnaryOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                             const std::shared_ptr<VkGPUBuffer> &outputBuffer) {
    this->inputBuffer = inputBuffer;
    this->outputBuffer = outputBuffer;
}

UnaryOperator::~UnaryOperator() {
    Destroy();
}

void UnaryOperator::Destroy() const {
    if (this->inputBuffer != nullptr) {
        this->inputBuffer->Destroy();
    }
    if (this->outputBuffer != nullptr) {
        this->outputBuffer->Destroy();
    }
}
