//
// Created by neo on 2025/3/13.
//

#ifndef RADIALBLURFILTER_H
#define RADIALBLURFILTER_H
#include "engine/effect/filters/BasicFilter.h"

struct RadialBlurFilterParams {
    BasicFilterParam imageSize;
    uint32_t centerX;
    uint32_t centerY;
    float strength;
    float rotationStrength;
    float sharpness;
};

class RadialBlurFilter final : public BasicFilter {
    RadialBlurFilterParams radialBlurFilterParams{};

public:
    RadialBlurFilter() = default;

    ~RadialBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetCenter(const uint32_t xPos, const uint32_t yPos) {
        this->radialBlurFilterParams.centerX = xPos;
        this->radialBlurFilterParams.centerY = yPos;
    }

    void SetStrength(const float strength) { this->radialBlurFilterParams.strength = strength; }

    void SetRotationStrength(const float rotationStrength) {
        this->radialBlurFilterParams.rotationStrength = rotationStrength;
    }

    void SetSharpness(const float sharpness) { this->radialBlurFilterParams.sharpness = sharpness; }

    void Destroy() override;
};


#endif //RADIALBLURFILTER_H
