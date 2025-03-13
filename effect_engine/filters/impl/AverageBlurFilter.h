//
// Created by Jun Li on 2025/3/13.
//

#ifndef AVERAGEBLURFILTER_H
#define AVERAGEBLURFILTER_H

#include "effect_engine/filters/BasicFilter.h"

struct AverageBlurFilterParams {
    BasicFilterParam imageSize;
    uint32_t blurRadius;
};

class AverageBlurFilter final : public BasicFilter {
    AverageBlurFilterParams blurParams = {};

public:
    AverageBlurFilter() = default;
    ~AverageBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetBlurRadius(uint32_t radius) { blurParams.blurRadius = radius; }

    void Destroy() override;
};

#endif // AVERAGEBLURFILTER_H
