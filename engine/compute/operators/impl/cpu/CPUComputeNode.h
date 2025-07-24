//
// Created by neo on 25-6-27.
//

#ifndef CPUCOMPUTENODE_H
#define CPUCOMPUTENODE_H
#include "core/gpu/compute_graph/IComputeGraphNode.h"

class CPUComputeNode : public IComputeGraphNode {
public:
  CPUComputeNode() = default;

  void Compute(VkCommandBuffer commandBuffer) override;

  void Destroy() override;

  ~CPUComputeNode() override = default;

  VkResult CreateComputeGraphNode() override;
};

#endif // CPUCOMPUTENODE_H
