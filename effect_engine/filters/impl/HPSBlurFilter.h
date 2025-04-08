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
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

public:
    HPSBlurFilter() = default;

    ~HPSBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   std::vector<FilterImageInfo> inputImageInfo,
                   std::vector<FilterImageInfo> outputImageInfo) override;

    void SetRadius(const int radius) { this->blurFilterParams.radius = radius; }

    void Destroy() override;
};

#endif //HPSBLURFILTER_H
