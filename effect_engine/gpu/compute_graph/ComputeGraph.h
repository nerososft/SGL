//
// Created by neo on 2025/3/11.
//

#ifndef COMPUTEGRAPH_H
#define COMPUTEGRAPH_H
#include <memory>
#include <vulkan/vulkan_core.h>

#include "IComputeGraphNode.h"
#include "SubComputeGraph.h"
#include "effect_engine/gpu/VkGPURenderPass.h"


class VkGPUContext;

class ComputeGraph {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::vector<std::shared_ptr<SubComputeGraph> > subGraphs;

public:
    explicit ComputeGraph(const std::shared_ptr<VkGPUContext> &gpuCtx);

    ~ComputeGraph() = default;

    void AddSubGraph(const std::shared_ptr<SubComputeGraph> &subGraph);

    [[nodiscard]] VkResult Compute() const;

    void Destroy() const;
};


#endif //COMPUTEGRAPH_H
