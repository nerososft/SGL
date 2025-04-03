//
// Created by neo on 25-4-2.
//

#ifndef HPSBLURFILTER_H
#define HPSBLURFILTER_H
#include "effect_engine/filters/BasicFilter.h"

struct HPSBlurFilterParams {
    BasicFilterParam imageSize;
    int radius;
};

class HPSBlurFilter final : public IFilter {
    HPSBlurFilterParams blurFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;

public:
    HPSBlurFilter() = default;

    ~HPSBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetRadius(const int radius) { this->blurFilterParams.radius = radius; }

    void Destroy() override;
};

#endif //HPSBLURFILTER_H
