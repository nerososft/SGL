//
// Created by neo on 2025/3/11.
//

#include "ComputeGraph.h"

#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "gpu/VkGPUContext.h"
#include "gpu/VkGPUHelper.h"
#include "log/Log.h"

ComputeGraph::ComputeGraph(const std::shared_ptr<VkGPUContext> &gpuCtx) {
    this->gpuCtx = gpuCtx;
}

void ComputeGraph::AddSubGraph(const std::shared_ptr<SubComputeGraph> &subGraph) {
    this->subGraphs.push_back(subGraph);
    computeDoneSemaphores.push_back(subGraph->GetComputeDoneSemaphore());
}

VkResult ComputeGraph::Compute() const {
    VkResult ret = VK_SUCCESS;
    std::vector<VkFence> waitFences;
    if (!this->subGraphs.empty()) {
        Logger() << Logger::DEBUG << "Subgraph Count: " << this->subGraphs.size() << std::endl;
        for (const auto &subGraph: this->subGraphs) {
            Logger() << Logger::DEBUG << "Execute Subgraph on " << subGraph->GetQueue().queueFamilyIndex << "-" <<
                    subGraph->GetQueue().
                    queueIndex << std::endl;
            ret = subGraph->Compute();
            waitFences.push_back(subGraph->GetComputeFence());
            if (ret != VK_SUCCESS) {
                Logger() << "SubComputeGraph::Compute failed!\n";
                return ret;
            }
        }
    }

    ret = vkWaitForFences(gpuCtx->GetCurrentDevice(),
                          waitFences.size(),
                          waitFences.data(),
                          VK_TRUE,
                          UINT64_MAX);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to wait fence, err=" << string_VkResult(ret) << std::endl;
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
