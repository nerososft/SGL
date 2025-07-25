//
// Created by neo on 25-6-27.
//

#ifndef RMSOPERATOR_H
#define RMSOPERATOR_H
#include "CPUComputeNode.h"
#include "core/compute/operators/IOperator.h"
#include "runtime/gpu/VkGPUBuffer.h"

class CPURmsNode final : public CPUComputeNode {
  std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
  float bias = 0.0;
  float *rms = nullptr;

public:
  CPURmsNode();

  ~CPURmsNode() override = default;

  VkResult CreateComputeGraphNode() override;

  void Compute(VkCommandBuffer commandBuffer) override;

  void Destroy() override;

  void SetInputBuffer(const std::shared_ptr<VkGPUBuffer> &inputBuffer) {
    this->inputBuffer = inputBuffer;
  }

  void SetRms(float *rms) { this->rms = rms; }

  void SetBias(const float bias) { this->bias = bias; }
};

class RMSOperator final : IOperator {
  std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
  float rms = 0.0;
  float bias = 0.0;

public:
  explicit RMSOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer);

  ~RMSOperator() override = default;

  void SetBias(const float bias) { this->bias = bias; }

  std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

  float *GetRms() { return &rms; }

  void Destroy() override;
};

#endif // RMSOPERATOR_H
