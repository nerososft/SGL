//
// Created by neo on 2025/3/11.
//

#ifndef COMPUTEGRAPH_H
#define COMPUTEGRAPH_H
#include <memory>
#include <vulkan/vulkan_core.h>

#include "SubComputeGraph.h"
#include "core/gpu/VkGPURenderPass.h"

class VkGPUContext;

class ComputeGraph {
  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
  std::vector<std::shared_ptr<SubComputeGraph>> subGraphs;
  std::vector<VkSemaphore> computeDoneSemaphores;

public:
  explicit ComputeGraph(const std::shared_ptr<VkGPUContext> &gpuCtx);

  ~ComputeGraph() = default;

  void AddSubGraph(const std::shared_ptr<SubComputeGraph> &subGraph);

  [[nodiscard]] VkResult Compute() const;

  [[nodiscard]] std::vector<VkSemaphore> GetComputeDoneSemaphores() const {
    return computeDoneSemaphores;
  }

  void Destroy() const;
};

#endif // COMPUTEGRAPH_H
