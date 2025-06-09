#pragma once
#ifndef ZIGZAGFILTER_H
#define ZIGZAGFILTER_H
#include <vulkan/vulkan_core.h>

#include "engine/effect/filters/BasicFilter.h"
#include "core/gpu/VkGPUContext.h"

struct ZigzagFilterParams {
    BasicFilterParam imageSize;
    int wavelength;
    int amplitude;
    int type_wave;
    float proportion;
};

class ZigzagFilter final : public BasicFilter {
    ZigzagFilterParams zigzagFilterParams{};

public:
    ZigzagFilter() = default;

    ~ZigzagFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetWavelength(const int wavelength) { this->zigzagFilterParams.wavelength = wavelength; }
    void SetAmplitude(const int amplitude) { this->zigzagFilterParams.amplitude = amplitude; }
    void SetTypeWave(const int type_wave) { this->zigzagFilterParams.type_wave = type_wave; }
    void SetPro(const float proportion) { this->zigzagFilterParams.proportion = proportion; }

    void Destroy() override;
};
#endif
