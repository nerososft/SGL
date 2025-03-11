//
// Created by neo on 2025/3/11.
//

#ifndef GAUSSIANBLURFILTER_H
#define GAUSSIANBLURFILTER_H
#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/filters/IFilter.h"
#include "effect_engine/gpu/compute_graph/ComputeGraph.h"

struct GaussianBlurFilterParams {
    BasicFilterParam imageSize;
    int radius;
};

class GaussianBlurFilter final : public IFilter {
    GaussianBlurFilterParams blurFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;

public:
    GaussianBlurFilter() = default;

    ~GaussianBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetRadius(const int radius) { this->blurFilterParams.radius = radius; }
};


#endif //GAUSSIANBLURFILTER_H
