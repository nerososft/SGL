//
// Created by neo on 2025/3/12.
//

#ifndef COLORSEPARATIONFILTER_H
#define COLORSEPARATIONFILTER_H
#include "effect_engine/filters/BasicFilter.h"

struct ColorSeparationFilterParams {
    BasicFilterParam imageSize;
    float redOffsetX;
    float greenOffsetX;
    float blueOffsetX;
};

class ColorSeparationFilter final : public BasicFilter {
    ColorSeparationFilterParams colorSeparationFilterParams{};

public:
    ColorSeparationFilter() = default;

    ~ColorSeparationFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   std::vector<FilterImageInfo> inputImageInfo,
                   std::vector<FilterImageInfo> outputImageInfo) override;

    void Destroy() override;

    void SetRedOffsetX(const float offsetX) { this->colorSeparationFilterParams.redOffsetX = offsetX; }
    void SetGreenOffsetX(const float offsetX) { this->colorSeparationFilterParams.greenOffsetX = offsetX; }
    void SetBlueOffsetX(const float offsetX) { this->colorSeparationFilterParams.blueOffsetX = offsetX; }
};

#endif //COLORSEPARATIONFILTER_H
