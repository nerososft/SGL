//
// Created by neo on 25-7-9.
//

#ifndef LOGSOFTMAXOPERATOR_H
#define LOGSOFTMAXOPERATOR_H

#include "engine/compute/operators/UnaryOperator.h"

struct LogSoftmaxOperatorParams {
    float sum;
};

class LogSoftmaxOperator final : public UnaryOperator {
    LogSoftmaxOperatorParams params{};

    float *sum = nullptr;

public:
    LogSoftmaxOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                       const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                       const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~LogSoftmaxOperator() override;

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void SetSum(float *sum) { this->sum = sum; }

    void Destroy() override;
};


#endif //LOGSOFTMAXOPERATOR_H
