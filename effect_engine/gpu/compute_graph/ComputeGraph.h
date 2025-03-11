//
// Created by neo on 2025/3/11.
//

#ifndef COMPUTEGRAPH_H
#define COMPUTEGRAPH_H
#include <memory>
#include <vulkan/vulkan_core.h>

#include "IComputeGraphNode.h"


class VkGPUContext;

class ComputeGraph {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    VkFence computeFence = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    std::vector<std::shared_ptr<IComputeGraphNode> > computeGraphNodes;

public:
    explicit ComputeGraph(const std::shared_ptr<VkGPUContext> &gpuCtx);

    ~ComputeGraph() = default;

    void AddComputeGraphNode(const std::shared_ptr<IComputeGraphNode> &computeGraphNode);

    VkResult Init();

    [[nodiscard]] VkResult Compute() const;
};


#endif //COMPUTEGRAPH_H
