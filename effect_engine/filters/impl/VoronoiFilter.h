//
// Created by neo on 2025/3/19.
//

#ifndef VORONOIFILTER_H
#define VORONOIFILTER_H
#include "effect_engine/filters/BasicFilter.h"


struct VoronoiFilterParams {
    BasicFilterParam imageSize;
    uint32_t gridSize;
    uint32_t enableEdge;
};

class VoronoiFilter final : public BasicFilter {
    VoronoiFilterParams voronoiFilterParams{};

public:
    VoronoiFilter() = default;

    ~VoronoiFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetGridSize(const uint32_t gridSize) { voronoiFilterParams.gridSize = gridSize; }

    void SetEnableEdge(const bool enableEdge) { voronoiFilterParams.enableEdge = enableEdge; }

    void Destroy() override;
};


#endif //VORONOIFILTER_H
