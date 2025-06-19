//
// Created by neo on 25-6-19.
//

#ifndef RMSNORMOPERATOR_H
#define RMSNORMOPERATOR_H
#include "engine/ml/operators/UnaryOperator.h"

struct RMSNormOperatorParams {
    float rms;
    float scale;
};

class RMSNormOperator final : public UnaryOperator {
    RMSNormOperatorParams params{};

public:
    RMSNormOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                    const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                    const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~RMSNormOperator() override;

    void SetScale(const float scale) { this->params.scale = scale; }

    void SetRMS(const float rms) { this->params.rms = rms; }

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void Destroy() override;
};

#endif //RMSNORMOPERATOR_H
