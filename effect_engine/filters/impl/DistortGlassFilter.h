//
// Created by neo on 2025/3/18.
//

#ifndef DISTORTGLASS_H
#define DISTORTGLASS_H

#include "effect_engine/filters/BasicFilter.h"

struct DistortGlassFilterParams {
    BasicFilterParam imageSize;
    float scale;
    float intensity;
    float zoom = 0;
};

class DistortGlassFilter final : public BasicFilter {
    DistortGlassFilterParams glassFilterParams{};

public:
    DistortGlassFilter() = default;

    ~DistortGlassFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetScale(const float scale) { this->glassFilterParams.scale = scale; }
    void SetIntensity(const float intensity) { this->glassFilterParams.intensity = intensity; }
    void SetZoom(const float zoom) { this->glassFilterParams.zoom = zoom; }

    void Destroy() override;
};


#endif //PALETTEKNIFEFILTER_H
