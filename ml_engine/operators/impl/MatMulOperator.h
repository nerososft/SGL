//
// Created by neo on 25-5-8.
//

#ifndef MATMUL_H
#define MATMUL_H
#include "ml_engine/operators/BinaryOperator.h"


class MatMulOperator final : public BinaryOperator {
public:
    MatMulOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::shared_ptr<VkGPUBuffer> &inputBuffer1,
                   const std::shared_ptr<VkGPUBuffer> &inputBuffer2,
                   const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    ~MatMulOperator() override;

    void Destroy() override;
};


#endif //MATMUL_H
