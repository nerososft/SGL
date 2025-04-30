#pragma once
#pragma once
#ifndef FACETFILTER_H
#define FACETFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "gpu_engine/gpu/VkGPUContext.h"

struct FacetFilterParams {
    BasicFilterParam imageSize;
    int radius;
    int intensityLevel;
};

class FacetFilter final : public BasicFilter {
    FacetFilterParams facetParams{};

public:
    FacetFilter() = default;

    ~FacetFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
        const std::vector<FilterImageInfo>& inputImageInfo,
        const std::vector<FilterImageInfo>& outputImageInfo) override;

    void SetRadius(const int radius) { this->facetParams.radius = radius; }
    void SetLevel(const int intensityLevel) { this->facetParams.intensityLevel = intensityLevel; }

    void Destroy() override;
};
#endif
