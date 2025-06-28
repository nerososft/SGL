//
// Created by neo on 25-6-28.
//

#include "Operator.h"

Operator::Operator(const std::shared_ptr<VkGPUContext> &gpuCtx) {
    this->gpuCtx = gpuCtx;
}

Operator::~Operator() {
    Operator::Destroy();
}

void Operator::Destroy() {
}
