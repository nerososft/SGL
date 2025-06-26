//
// Created by neo on 25-6-19.
//

#ifndef LAYERNORMOPERATOR_H
#define LAYERNORMOPERATOR_H
#include <engine/ml/operators/UnaryOperator.h>

struct LayerNormOperatorParams {
    float avg;
    float variance;
    float epsilon;
    bool weightEnable;
    bool biasEnable;
};

class LayerNormOperator final : public UnaryOperator {
    LayerNormOperatorParams params{};

    std::shared_ptr<VkGPUBuffer> weightBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> biasBuffer = nullptr;

public:
    LayerNormOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                      const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                      const std::shared_ptr<VkGPUBuffer> &weightBuffer,
                      const std::shared_ptr<VkGPUBuffer> &biasBuffer,
                      const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~LayerNormOperator() override;

    void SetAvg(const float avg) { this->params.avg = avg; }

    void SetVariance(const float variance) { this->params.variance = variance; }

    void SetEpsilon(const float epsilon) { this->params.epsilon = epsilon; }

    void SetWeightEnable(const bool weightEnable) { this->params.weightEnable = weightEnable; }

    void SetBiasEnable(const bool biasEnable) { this->params.biasEnable = biasEnable; }

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void Destroy() override;
};

#endif //LAYERNORMOPERATOR_H
