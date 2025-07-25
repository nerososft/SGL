//
// Created by neo on 25-7-9.
//

#ifndef MAXOPERATOR_H
#define MAXOPERATOR_H

#include "CPUComputeNode.h"
#include "core/compute/operators/IOperator.h"
#include "runtime/gpu/VkGPUBuffer.h"

class CPUMaxNode final : public CPUComputeNode {
  std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
  float *max = nullptr;

public:
  CPUMaxNode();

  ~CPUMaxNode() override = default;

  VkResult CreateComputeGraphNode() override;

  void Compute(VkCommandBuffer commandBuffer) override;

  void Destroy() override;

  void SetInputBuffer(const std::shared_ptr<VkGPUBuffer> &inputBuffer) {
    this->inputBuffer = inputBuffer;
  }

  void SetMax(float *max) { this->max = max; }
};

class MaxOperator final : IOperator {
  std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
  float *max = nullptr;
  float innerMax = 0.0f;

public:
  explicit MaxOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer);

  ~MaxOperator() override = default;

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  [[nodiscard]] float *GetMax() const { return max; }

  float *GetInnerMax() { return &innerMax; }

  void Destroy() override;

  void SetMax(float *max) { this->max = max; }
};

#endif // MAXOPERATOR_H
