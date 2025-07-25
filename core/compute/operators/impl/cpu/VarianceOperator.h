//
// Created by neo on 25-6-27.
//

#ifndef VARIANCEOPERATOR_H
#define VARIANCEOPERATOR_H
#include "CPUComputeNode.h"
#include "core/compute/operators/IOperator.h"
#include "runtime/gpu/VkGPUBuffer.h"

class CPUVarianceNode final : public CPUComputeNode {
  std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
  float *variance = nullptr;
  float *avg = nullptr;

public:
  CPUVarianceNode();

  ~CPUVarianceNode() override = default;

  VkResult CreateComputeGraphNode() override;

  void Compute(VkCommandBuffer commandBuffer) override;

  void Destroy() override;

  void SetInputBuffer(const std::shared_ptr<VkGPUBuffer> &inputBuffer) {
    this->inputBuffer = inputBuffer;
  }

  void SetVariance(float *variance) { this->variance = variance; }

  void SetAvg(float *avg) { this->avg = avg; }
};

class VarianceOperator final : IOperator {
  std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
  float variance = 0.0;
  float *avg = nullptr;

public:
  explicit VarianceOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                            float *avg);

  ~VarianceOperator() override = default;

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  float *GetVariance() { return &variance; }

  void Destroy() override;
};

#endif // VARIANCEOPERATOR_H
