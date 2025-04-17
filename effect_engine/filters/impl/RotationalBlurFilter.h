#pragma once
#ifndef ROTATIONALBLURFILTER_H
#define ROTATIONALBLURFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"


struct RotationalBlurFilterParams {
    BasicFilterParam imageSize;
    float angle;
    int strength;
};
class RotationalBlurFilter final : public IFilter {
    RotationalBlurFilterParams rotationblurFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
public:
    RotationalBlurFilter() = default;

    ~RotationalBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;
    void SetStrength(const int _strength) { this->rotationblurFilterParams.strength = _strength; }
    void SetAngle(const float _angle) { this->rotationblurFilterParams.angle = _angle; }
    void Destroy() override;
};
#endif
