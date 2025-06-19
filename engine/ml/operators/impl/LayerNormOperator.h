//
// Created by neo on 25-6-19.
//

#ifndef LAYERNORMOPERATOR_H
#define LAYERNORMOPERATOR_H
#include <engine/ml/operators/UnaryOperator.h>

struct LayerNormOperatorParams {
    float avg;
    float variance;
    float scale;
    float epsilon;
    float bias;
};

class LayerNormOperator final : public UnaryOperator {
    LayerNormOperatorParams params{};

public:
    LayerNormOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                      const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                      const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~LayerNormOperator() override;

    void SetScale(const float scale) { this->params.scale = scale; }

    void SetAvg(const float avg) { this->params.avg = avg; }

    void SetVariance(const float variance) { this->params.variance = variance; }

    void SetEpsilon(const float epsilon) { this->params.epsilon = epsilon; }

    void SetBias(const float bias) { this->params.bias = bias; }

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void Destroy() override;
};

#endif //LAYERNORMOPERATOR_H
