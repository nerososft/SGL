#pragma once
#ifndef ROTATIONBLURFILTER_H
#define ROTATIONBLURFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"

struct RotationBlurFilterParams {
    BasicFilterParam imageSize;
    float x;
    float y;
    float a;
    float b;
    float ina;
    float inb;
    int strength;
    float angle;
};
class RotationBlurFilter final : public BasicFilter {
    RotationBlurFilterParams rotationblurFilterParams{};

public:
    RotationBlurFilter() = default;

    ~RotationBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
        std::vector<FilterImageInfo> inputImageInfo,
        std::vector<FilterImageInfo> outputImageInfo) override;

    void SetCenterX(const float _x) { this->rotationblurFilterParams.x = _x; }
    void SetCenterY(const float _y) { this->rotationblurFilterParams.y = _y; }
    void SetA(const float _a) { this->rotationblurFilterParams.a = _a; }
    void SetB(const float _b) { this->rotationblurFilterParams.b = _b; }
    void SetinA(const float _ina) { this->rotationblurFilterParams.ina = _ina; }
    void SetinB(const float _inb) { this->rotationblurFilterParams.ina = _inb; }
    void SetStrength(const int _strength) { this->rotationblurFilterParams.strength = _strength; }
    void SetAngle(const float _angle) { this->rotationblurFilterParams.angle = _angle; }
    void Destroy() override;
};
#endif