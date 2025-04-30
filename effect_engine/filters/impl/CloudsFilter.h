#pragma once
#ifndef CLOUDSFILTER_H
#define CLOUDSFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "gpu_engine/gpu/VkGPUContext.h"
#include <gpu_engine/gpu/VkGPUBuffer.h>

struct CloudsFilterParams {
    BasicFilterParam imageSize;
    int type;
};

class CloudsFilter final : public IFilter {
    CloudsFilterParams cloudsFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
    std::shared_ptr<VkGPUBuffer> lookupBuffer = nullptr;
    int* lookup = nullptr;
    int size = 0;

public:
    CloudsFilter() = default;

    ~CloudsFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
        const std::vector<FilterImageInfo>& inputImageInfo,
        const std::vector<FilterImageInfo>& outputImageInfo) override;;

    void SetLookup(int* _lookup, int size) {
        this->lookup = _lookup;
        this->size = size;
    }
    void SetType(int type)
    {
        this->cloudsFilterParams.type = type;
    }

    void Destroy() override;
};
#endif