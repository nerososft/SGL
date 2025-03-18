//
// Created by Jun Li on 2025/3/13.
//

#ifndef AVERAGEBLURFILTER_H
#define AVERAGEBLURFILTER_H
#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/filters/IFilter.h"

struct AverageBlurFilterParams {
    BasicFilterParam imageSize;
    uint32_t blurRadius;
};

class AverageBlurFilter final : public IFilter {
    AverageBlurFilterParams blurParams = {};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;

public:
    AverageBlurFilter() = default;

    ~AverageBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetBlurRadius(const uint32_t radius) { blurParams.blurRadius = radius; }

    void Destroy() override;
};

#endif // AVERAGEBLURFILTER_H
