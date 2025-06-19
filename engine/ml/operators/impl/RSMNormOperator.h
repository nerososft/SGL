//
// Created by neo on 25-6-19.
//

#ifndef RSMNORMOPERATOR_H
#define RSMNORMOPERATOR_H
#include "engine/ml/operators/UnaryOperator.h"

struct RSMNormOperatorParams {
    float rsm;
    float scale;
};

class RSMNormOperator final : public UnaryOperator {
    RSMNormOperatorParams params{};

public:
    RSMNormOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                    const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                    const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~RSMNormOperator() override;

    void SetScale(const float scale) { this->params.scale = scale; }

    void SetRSM(const float rsm) { this->params.rsm = rsm; }

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void Destroy() override;
};

#endif //RSMNORMOPERATOR_H
