//
// Created by neo on 2025/3/11.
//

#include "ComputeGraph.h"

#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/gpu/VkGPUContext.h"
#include "effect_engine/gpu/VkGPUHelper.h"
#include "effect_engine/log/Log.h"

ComputeGraph::ComputeGraph(const std::shared_ptr<VkGPUContext> &gpuCtx) {
    this->gpuCtx = gpuCtx;
}

void ComputeGraph::AddSubGraph(const std::shared_ptr<SubComputeGraph> &subGraph) {
    this->subGraphs.push_back(subGraph);
}

VkResult ComputeGraph::Compute() const {
    VkResult ret = VK_SUCCESS;
    if (!this->subGraphs.empty()) {
        for (const auto &subGraph: this->subGraphs) {
            ret = subGraph->Compute();
            if (ret != VK_SUCCESS) {
                Logger() << "SubComputeGraph::Compute failed!\n";
                return ret;
            }
        }
    }

    ret = vkQueueWaitIdle(gpuCtx->DispatchQueue(VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_GRAPHICS_BIT).queue);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to wait idle, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}

void ComputeGraph::Destroy() const {
    for (const auto &subGraph: this->subGraphs) {
        subGraph->Destroy();
    }
    this->gpuCtx->Reset();
}
