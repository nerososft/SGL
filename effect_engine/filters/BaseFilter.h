//
// Created by 1234 on 2025/3/6.
//

#ifndef BASEFILTER_H
#define BASEFILTER_H
#include <vulkan/vulkan_core.h>

#include "IFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"
#include "effect_engine/gpu/compute_graph/ComputeGraph.h"

struct BasicFilterParam {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint32_t bytesPerLine;
};

struct FilterParams {
    size_t paramsSize;
    void *paramsData;
    std::string shaderPath;
};

class BaseFilter : public IFilter {
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;

public:
    BaseFilter() = default;

    ~BaseFilter() override = default;

    VkResult DoApply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                     const std::string &name,
                     VkDeviceSize bufferSize,
                     uint32_t width,
                     uint32_t height,
                     VkBuffer inputBuffer,
                     VkBuffer outputBuffer,
                     const FilterParams &filterParams);

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;
};


#endif //BASEFILTER_H
