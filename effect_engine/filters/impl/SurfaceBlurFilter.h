//
// Created by Jun Li on 2025/3/13.
//

// SurfaceBlurFilter.h
#ifndef SURFACEBLURFILTER_H
#define SURFACEBLURFILTER_H

#include "effect_engine/filters/BasicFilter.h"

struct SurfaceBlurFilterParams {
    BasicFilterParam imageSize;
    uint32_t blurRadius;
    float threshold; // Normalized threshold (0.0 - 1.0)
};

class SurfaceBlurFilter final : public BasicFilter {
    SurfaceBlurFilterParams blurParams = {};

public:
    SurfaceBlurFilter() = default;

    ~SurfaceBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   std::vector<FilterImageInfo> inputImageInfo,
                   std::vector<FilterImageInfo> outputImageInfo) override;

    void SetBlurRadius(const uint32_t radius) { blurParams.blurRadius = radius; }

    void SetThreshold(const uint32_t threshold) {
        blurParams.threshold = static_cast<float>(threshold) / 255.0f;
    }

    void Destroy() override;
};

#endif // SURFACEBLURFILTER_H
