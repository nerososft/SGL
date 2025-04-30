#pragma once
#ifndef PINCHFILTER_H
#define PINCHFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "gpu_engine/gpu/VkGPUContext.h"

struct PinchFilterParams {
    BasicFilterParam imageSize;
    int amount;
};

class PinchFilter final : public BasicFilter {
    PinchFilterParams pinchFilterParams{};

public:
    PinchFilter() = default;

    ~PinchFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetAmount(const int amount) { this->pinchFilterParams.amount = amount; }

    void Destroy() override;
};
#endif
