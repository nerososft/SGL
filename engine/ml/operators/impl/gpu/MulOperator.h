//
// Created by neo on 25-6-30.
//

#ifndef MULOPERATOR_H
#define MULOPERATOR_H
#include "engine/ml/operators/BinaryOperator.h"


class MulOperator final : public BinaryOperator {
public:
    MulOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                    const std::shared_ptr<VkGPUBuffer> &inputBuffer1,
                    const std::shared_ptr<VkGPUBuffer> &inputBuffer2,
                    const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~MulOperator() override;

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void Destroy() override;
};


#endif //MULOPERATOR_H
