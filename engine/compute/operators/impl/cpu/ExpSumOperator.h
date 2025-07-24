//
// Created by neo on 25-7-9.
//

#ifndef EXPSUMOPERATOR_H
#define EXPSUMOPERATOR_H

#include "CPUComputeNode.h"
#include "MaxOperator.h"
#include "core/gpu/VkGPUBuffer.h"
#include "engine/compute/operators/IOperator.h"

class CPUExpSumNode final : public CPUComputeNode {
  std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
  float *sum = nullptr;
  float *max = nullptr;

public:
  CPUExpSumNode();

  ~CPUExpSumNode() override = default;

  VkResult CreateComputeGraphNode() override;

  void Compute(VkCommandBuffer commandBuffer) override;

  void Destroy() override;

  void SetInputBuffer(const std::shared_ptr<VkGPUBuffer> &inputBuffer) {
    this->inputBuffer = inputBuffer;
  }

  void SetSum(float *sum) { this->sum = sum; }

  void SetMax(float *max) { this->max = max; }
};

class ExpSumOperator final : IOperator {
  std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
  float *sum = nullptr;
  float *max = nullptr;
  float innerSum = 0.0f;

public:
  explicit ExpSumOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer);

  ~ExpSumOperator() override = default;

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  [[nodiscard]] float *GetSum() const { return sum; }

  float *GetInnerSum() { return &innerSum; }

  void Destroy() override;

  void SetSum(float *sum) { this->sum = sum; }

  void SetMax(float *max) { this->max = max; };
};

#endif // EXPSUMOPERATOR_H
