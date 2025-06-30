//
// Created by neo on 25-6-30.
//

#ifndef DOTPRODOPERATOR_H
#define DOTPRODOPERATOR_H
#include "engine/ml/operators/BinaryOperator.h"


class DotProdOperator final : public BinaryOperator {
public:
    DotProdOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                    const std::shared_ptr<VkGPUBuffer> &inputBuffer1,
                    const std::shared_ptr<VkGPUBuffer> &inputBuffer2,
                    const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~DotProdOperator() override;

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void Destroy() override;
};


#endif //DOTPRODOPERATOR_H
