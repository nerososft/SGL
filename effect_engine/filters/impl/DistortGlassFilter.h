//
// Created by neo on 2025/3/18.
//

#ifndef DISTORTGLASS_H
#define DISTORTGLASS_H
#include <effect_engine/gpu/VkGPUBuffer.h>

#include "effect_engine/filters/BasicFilter.h"

struct DistortGlassFilterParams {
    BasicFilterParam imageSize;
    float scale;       // 噪声缩放
    float intensity;   // 扭曲强度
    float zoom = 0;        // 缩放因子
};

class DistortGlassFilter final : public BasicFilter {
    DistortGlassFilterParams glassFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
public:
    DistortGlassFilter() = default;

    ~DistortGlassFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetScale(const float _scale) { this->glassFilterParams.scale = _scale; }
    void SetIntensity(const float _intensity) { this->glassFilterParams.intensity = _intensity; }
    void SetZoom(const float _zoom) { this->glassFilterParams.zoom = _zoom; }

    void Destroy() override;
};


#endif //PALETTEKNIFEFILTER_H
