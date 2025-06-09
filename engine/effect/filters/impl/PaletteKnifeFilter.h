//
// Created by neo on 2025/3/18.
//

#ifndef PPALETTEKNIFEFILTER_H
#define PPALETTEKNIFEFILTER_H
#include <core/gpu/VkGPUBuffer.h>

#include "engine/effect/filters/BasicFilter.h"

struct PaletteKnifeFilterParams {
    BasicFilterParam imageSize;
    int radius;
    int quantScale;
};

class PaletteKnifeFilter final : public BasicFilter {
    PaletteKnifeFilterParams paletteKnifeFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
    std::shared_ptr<VkGPUBuffer> qBuffer = nullptr;

public:
    PaletteKnifeFilter() = default;

    ~PaletteKnifeFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetRadius(const int radius) { this->paletteKnifeFilterParams.radius = radius; }

    void SetQuantScale(const int quantScale) { this->paletteKnifeFilterParams.quantScale = quantScale; }

    void Destroy() override;
};

#endif //PALETTEKNIFEFILTER_H
