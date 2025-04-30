#pragma once
#ifndef SPHERIZEFILTER_H
#define SPHERIZEFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "gpu_engine/gpu/VkGPUContext.h"

struct SpherizeFilterParams {
    BasicFilterParam imageSize;
    int alpha;
    int type;
};

class SpherizeFilter final : public BasicFilter {
    SpherizeFilterParams spherizeFilterParams{};

public:
    SpherizeFilter() = default;

    ~SpherizeFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetAlpha(const int alpha) { this->spherizeFilterParams.alpha = alpha; }
    void SetType(const int type) { this->spherizeFilterParams.type = type; }

    void Destroy() override;
};
#endif
