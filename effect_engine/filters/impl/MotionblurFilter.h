#pragma once
#ifndef MOTIONBLURFILTER_H
#define MOTIONBLURFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "gpu_engine/gpu/VkGPUContext.h"

struct MotionblurFilterParams {
    BasicFilterParam imageSize;
    int distance;
    int angle;
    float proportion;
};

class MotionblurFilter final : public BasicFilter {
    MotionblurFilterParams motionblurFilterParams{};

public:
    MotionblurFilter() = default;

    ~MotionblurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
        const std::vector<FilterImageInfo>& inputImageInfo,
        const std::vector<FilterImageInfo>& outputImageInfo) override;

    void SetDistance(int distance) { this->motionblurFilterParams.distance = distance; }
    void SetAngle(int angle) { this->motionblurFilterParams.angle = angle; }
    void SetPro(float proportion) { this->motionblurFilterParams.proportion = proportion; }

    void Destroy() override;
};
#endif
