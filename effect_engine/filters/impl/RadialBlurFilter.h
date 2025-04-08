//
// Created by neo on 2025/3/13.
//

#ifndef RADIALBLURFILTER_H
#define RADIALBLURFILTER_H
#include "effect_engine/filters/BasicFilter.h"

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
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
    std::vector<FilterImageInfo> inputImageInfo,
    std::vector<FilterImageInfo> outputImageInfo) override {
        return VK_SUCCESS;
    }

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
