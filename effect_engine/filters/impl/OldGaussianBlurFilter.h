//
// Created by neo on 2025/3/11.
//

#ifndef OLDGAUSSIANBLURFILTER_H
#define OLDGAUSSIANBLURFILTER_H
#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/filters/IFilter.h"
#include "effect_engine/gpu/compute_graph/ComputeGraph.h"

#define MAX_RADIUS (256)

struct OldGaussianBlurFilterParams {
    BasicFilterParam imageSize;
    int radius;
};

class OldGaussianBlurFilter final : public IFilter {
    OldGaussianBlurFilterParams blurFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

public:
    OldGaussianBlurFilter() = default;

    ~OldGaussianBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetRadius(const int radius) { this->blurFilterParams.radius = radius; }

    void Destroy() override;
};


#endif //OLDGAUSSIANBLURFILTER_H
