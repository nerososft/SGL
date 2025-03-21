//
// Created by neo on 2025/3/18.
//

#ifndef PPALETTEKNIFEFILTER_H
#define PPALETTEKNIFEFILTER_H
#include <effect_engine/gpu/VkGPUBuffer.h>

#include "effect_engine/filters/BasicFilter.h"

struct PaletteKnifeFilterParams {
    BasicFilterParam imageSize;
    int radius;
    int quantScale;
};

class PaletteKnifeFilter final : public BasicFilter {
    PaletteKnifeFilterParams paletteKnifeFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<VkGPUBuffer> qBuffer = nullptr;

public:
    PaletteKnifeFilter() = default;

    ~PaletteKnifeFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetRadius(const int radius) { this->paletteKnifeFilterParams.radius = radius; }

    void SetQuantScale(const int quantScale) { this->paletteKnifeFilterParams.quantScale = quantScale; }

    void Destroy() override;
};


#endif //PALETTEKNIFEFILTER_H
