//
// Created by neo on 25-7-21.
//

#ifndef EXPOPERATOR_H
#define EXPOPERATOR_H
#include "engine/compute/operators/UnaryOperator.h"


class ExpOperator final : public UnaryOperator {
public:
    ExpOperator(const std::shared_ptr<VkGPUContext>& gpuCtx,
                const std::shared_ptr<VkGPUBuffer>& inputBuffer,
                const std::shared_ptr<VkGPUBuffer>& outputBuffer);

    ~ExpOperator() override;

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void Destroy() override;
};

#endif // EXPOPERATOR_H
