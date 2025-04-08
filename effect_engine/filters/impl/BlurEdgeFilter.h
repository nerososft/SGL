//
// Created by neo on 2025/3/18.
//

#ifndef BLUREDGE_H
#define BLUREDGE_H
#include <effect_engine/gpu/VkGPUBuffer.h>

#include "effect_engine/filters/BasicFilter.h"

struct BlurEdgeFilterParams {
    BasicFilterParam imageSize;
    int radius;
    int sigma;
    int k;
};

class BlurEdgeFilter final : public BasicFilter {
    BlurEdgeFilterParams blurEdgeFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
public:
    BlurEdgeFilter() = default;

    ~BlurEdgeFilter() override = default;



    VkResult Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
        std::vector<FilterImageInfo> inputImageInfo,
        std::vector<FilterImageInfo> outputImageInfo) override;

    void SetRadius(const int radius) { this->blurEdgeFilterParams.radius = radius; }

    void SetSigma(const int sigma) { this->blurEdgeFilterParams.sigma = sigma; }
    void SetKernel(const int _k) { this->blurEdgeFilterParams.k = _k; }

    void Destroy() override;
};


#endif //PALETTEKNIFEFILTER_H
