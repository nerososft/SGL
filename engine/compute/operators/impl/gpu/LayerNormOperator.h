//
// Created by neo on 25-6-19.
//

#ifndef LAYERNORMOPERATOR_H
#define LAYERNORMOPERATOR_H
#include <cassert>
#include <engine/compute/operators/UnaryOperator.h>

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

  float *avg = nullptr;
  float *variance = nullptr;

public:
  LayerNormOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                    const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                    const std::shared_ptr<VkGPUBuffer> &weightBuffer,
                    const std::shared_ptr<VkGPUBuffer> &biasBuffer,
                    const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  ~LayerNormOperator() override;

  void SetAvg(float *avg) { this->avg = avg; }

  void SetVariance(float *variance) { this->variance = variance; }

  void SetEpsilon(const float epsilon) { this->params.epsilon = epsilon; }

  void SetWeightEnable(const bool weightEnable) {
    this->params.weightEnable = weightEnable;
  }

  void SetBiasEnable(const bool biasEnable) {
    this->params.biasEnable = biasEnable;
  }

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void Destroy() override;
};

#endif // LAYERNORMOPERATOR_H
