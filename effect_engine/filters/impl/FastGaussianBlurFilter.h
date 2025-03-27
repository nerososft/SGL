//
// Created by neo on 2025/3/27.
//

#ifndef FASTGAUSSIANBLURFILTER_H
#define FASTGAUSSIANBLURFILTER_H

#define MAX_RADIUS (256)
#include "ScaleFilter.h"
#include "effect_engine/filters/BasicFilter.h"

struct FastGaussianBlurFilterParams {
    BasicFilterParam imageSize;
    int radius;
};

class FastGaussianBlurFilter final : public IFilter {
    FastGaussianBlurFilterParams blurFilterParams{};
    ScaleFilterParams scaleParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;

public:
    FastGaussianBlurFilter() = default;

    ~FastGaussianBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetRadius(const int radius) { this->blurFilterParams.radius = radius; }

    void Destroy() override;
};


#endif //FASTGAUSSIANBLURFILTER_H
