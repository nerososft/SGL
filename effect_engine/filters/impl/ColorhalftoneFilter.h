#pragma once
#ifndef COLORHALFTONEFILTER_H
#define COLORHALFTONEFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "gpu_engine/gpu/VkGPUContext.h"
#include <gpu_engine/gpu/VkGPUBuffer.h>

struct ColorhalftoneFilterParams {
    BasicFilterParam imageSize;
    float cyanAngle;
    float yellowAngle;
    float magentaAngle;
    float radius;
};

class ColorhalftoneFilter final : public IFilter {
    ColorhalftoneFilterParams colorhalftoneFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
    std::shared_ptr<VkGPUBuffer> lookupBuffer = nullptr;
    float *lookup = nullptr;
    int size = 0;

public:
    ColorhalftoneFilter() = default;

    ~ColorhalftoneFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetColor(float cyan, float yellow, float magenta, float radius) {
        this->colorhalftoneFilterParams.cyanAngle = cyan;
        this->colorhalftoneFilterParams.yellowAngle = yellow;
        this->colorhalftoneFilterParams.magentaAngle = magenta;
        this->colorhalftoneFilterParams.radius = radius;
    }

    void SetLookup(float *lookup, const int size) {
        this->lookup = lookup;
        this->size = size;
    }

    void Destroy() override;
};
#endif
