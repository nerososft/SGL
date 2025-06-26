//
// Created by neo on 25-5-25.
//

#ifndef SOFTMAXOPERATOR_H
#define SOFTMAXOPERATOR_H

#include "engine/ml/operators/UnaryOperator.h"

struct SoftmaxOperatorParams {
    float sum;
};

class SoftmaxOperator final : public UnaryOperator {
    SoftmaxOperatorParams params{};

public:
    SoftmaxOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                    const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                    const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~SoftmaxOperator() override;

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void SetSum(const float sum) { this->params.sum = sum; }

    void Destroy() override;
};

#endif //SOFTMAXOPERATOR_H
