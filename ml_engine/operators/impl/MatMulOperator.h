//
// Created by neo on 25-5-8.
//

#ifndef MATMUL_H
#define MATMUL_H
#include "ml_engine/operators/BinaryOperator.h"

struct MatMulOperatorParams {
    uint32_t width1;
    uint32_t height1;
    uint32_t width2;
    uint32_t height2;
};

class MatMulOperator final : public BinaryOperator {
    MatMulOperatorParams params{};

public:
    MatMulOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::shared_ptr<VkGPUBuffer> &inputBuffer1,
                   const std::shared_ptr<VkGPUBuffer> &inputBuffer2,
                   const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void SetMat1Size(const uint32_t width, const uint32_t height) {
        this->params.width1 = width;
        this->params.height1 = height;
    }

    void SetMat2Size(const uint32_t width, const uint32_t height) {
        this->params.width2 = width;
        this->params.height2 = height;
    }

    ~MatMulOperator() override;

    void Destroy() override;
};


#endif //MATMUL_H
