//
// Created by neo on 25-5-25.
//

#ifndef SIGMODOPERATOR_H
#define SIGMODOPERATOR_H

#include "ml_engine/operators/UnaryOperator.h"

class SigmoidOperator final : public UnaryOperator {
public:
    SigmoidOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                    const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                    const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~SigmoidOperator() override;

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void Destroy() override;
};

#endif //SIGMODOPERATOR_H
