//
// Created by neo on 25-6-19.
//

#ifndef RMSNORMOPERATOR_H
#define RMSNORMOPERATOR_H
#include "engine/compute/operators/UnaryOperator.h"

struct RMSNormOperatorParams {
  float rms;
  float scale;
  float epsilon;
};

class RMSNormOperator final : public UnaryOperator {
  RMSNormOperatorParams params{};

  float *rms = nullptr;

public:
  RMSNormOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                  const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                  const std::shared_ptr<VkGPUBuffer> &outputBuffer);

  ~RMSNormOperator() override;

  void SetScale(const float scale) { this->params.scale = scale; }

  void SetRMS(float *rms) { this->rms = rms; }

  void SetEpsilon(const float epsilon) { this->params.epsilon = epsilon; }

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  void Destroy() override;
};

#endif // RMSNORMOPERATOR_H
