//
// Created by neo on 25-4-7.
//

#ifndef SUBCOMPUTEGRAPH_H
#define SUBCOMPUTEGRAPH_H
#include <vulkan/vulkan_core.h>

#include "IComputeGraphNode.h"
#include "core/gpu/VkGPUContext.h"

class SubComputeGraph {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    DeviceQueue queue{};
    VkFence computeFence = VK_NULL_HANDLE;
    VkSemaphore computeDoneSemaphore = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    std::vector<std::shared_ptr<IComputeGraphNode> > computeGraphNodes;
    std::vector<std::shared_ptr<SubComputeGraph> > dependence;

public:
    explicit SubComputeGraph(const std::shared_ptr<VkGPUContext> &gpuCtx);

    ~SubComputeGraph() = default;

    VkResult Init();

    void AddDependence(const std::shared_ptr<SubComputeGraph> &dependence);

    void AddComputeGraphNode(const std::shared_ptr<IComputeGraphNode> &computeGraphNode);

    [[nodiscard]] VkSemaphore GetComputeDoneSemaphore() const { return this->computeDoneSemaphore; }

    [[nodiscard]] DeviceQueue GetQueue() const { return this->queue; }
    [[nodiscard]] VkFence GetComputeFence() const { return this->computeFence; }

    [[nodiscard]] VkResult Compute() const;

    void Destroy();
};

#endif //SUBCOMPUTEGRAPH_H
