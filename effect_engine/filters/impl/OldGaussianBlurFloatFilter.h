//
// Created by neo on 2025/3/11.
//

#ifndef OLDGAUSSIANBLURFILTERF_H
#define OLDGAUSSIANBLURFILTERF_H
#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/filters/IFilter.h"
#include "effect_engine/gpu/compute_graph/ComputeGraph.h"

#define MAX_RADIUS (256)

struct OldGaussianBlurFloatFilterParams {
    BasicFilterParam imageSize;
    int radius;
};

class OldGaussianBlurFloatFilter final : public IFilter {
    OldGaussianBlurFloatFilterParams blurFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

public:
    OldGaussianBlurFloatFilter() = default;

    ~OldGaussianBlurFloatFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetRadius(const int radius) { this->blurFilterParams.radius = radius; }

    void Destroy() override;
};


#endif //OLDGAUSSIANBLURFILTER_H
