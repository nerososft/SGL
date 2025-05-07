//
// Created by neo on 25-5-7.
//

#ifndef RELUOPERATOR_H
#define RELUOPERATOR_H
#include "compute_engine/operators/UnaryOperator.h"

class ReLUOperator final : public UnaryOperator {
public:
    ReLUOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                 const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~ReLUOperator() override;

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void Destroy() override;
};


#endif //RELUOPERATOR_H
