//
// Created by neo on 2025/3/11.
//

#ifndef GAUSSIANBLURFILTER_H
#define GAUSSIANBLURFILTER_H
#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/filters/IFilter.h"
#include "effect_engine/gpu/VkGPUBuffer.h"
#include "effect_engine/gpu/compute_graph/ComputeGraph.h"

#define MAX_RADIUS (256)

struct GaussianBlurFilterParams {
    BasicFilterParam imageSize;
    int radius;
};

class GaussianBlurFilter final : public IFilter {
    GaussianBlurFilterParams blurFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

private:
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;

    static std::vector<float> CalculateWeights();

    std::shared_ptr<VkGPUBuffer> weightBuffer = nullptr;

public:
    GaussianBlurFilter() = default;

    ~GaussianBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   std::vector<FilterImageInfo> inputImageInfo,
                   std::vector<FilterImageInfo> outputImageInfo) override;

    void SetRadius(const int radius) { this->blurFilterParams.radius = radius; }

    void Destroy() override;
};


#endif //GAUSSIANBLURFILTER_H
