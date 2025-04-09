#pragma once
#ifndef PATHBLURFILTER_H
#define PATHBLURFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"
#include <effect_engine/gpu/VkGPUBuffer.h>

struct pathBlurFilterParams {
    BasicFilterParam imageSize;
    int amount;
};

class pathBlurFilter final : public IFilter {
    pathBlurFilterParams pathblurFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
    std::shared_ptr<VkGPUBuffer> vecBuffer = nullptr;
    float *vec = nullptr;
    int k_size = 0;

public:
    pathBlurFilter() = default;

    ~pathBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetK(float *vec, int size) {
        this->vec = vec;
        this->k_size = size;
    }

    void SetAmount(const int amount) { this->pathblurFilterParams.amount = amount; }

    void Destroy() override;
};
#endif
