#pragma once
#ifndef POLARCOORDINATESFILTER_H
#define POLARCOORDINATESFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"

struct PolarCoordinatesFilterParams {
    BasicFilterParam imageSize;
    int type;
};

class PolarCoordinatesFilter final : public BasicFilter {
    PolarCoordinatesFilterParams polarcoordinatesFilterParams{};

public:
    PolarCoordinatesFilter() = default;

    ~PolarCoordinatesFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
        const std::vector<FilterImageInfo>& inputImageInfo,
        const std::vector<FilterImageInfo>& outputImageInfo) override;

    void SetType(int type) { this->polarcoordinatesFilterParams.type = type; }

    void Destroy() override;
};
#endif
