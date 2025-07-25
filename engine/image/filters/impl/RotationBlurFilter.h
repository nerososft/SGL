#pragma once
#ifndef ROTATIONBLURFILTER_H
#define ROTATIONBLURFILTER_H
#include <vulkan/vulkan_core.h>

#include "core/gpu/VkGPUContext.h"
#include "engine/image/filters/BasicFilter.h"

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

class RotationBlurFilter final : public IFilter {
  RotationBlurFilterParams rotationblurFilterParams{};
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

public:
  RotationBlurFilter() = default;

  ~RotationBlurFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetCenterX(const float _x) { this->rotationblurFilterParams.x = _x; }
  void SetCenterY(const float _y) { this->rotationblurFilterParams.y = _y; }
  void SetA(const float _a) { this->rotationblurFilterParams.a = _a; }
  void SetB(const float _b) { this->rotationblurFilterParams.b = _b; }
  void SetinA(const float _ina) { this->rotationblurFilterParams.ina = _ina; }
  void SetinB(const float _inb) { this->rotationblurFilterParams.inb = _inb; }
  void SetStrength(const int _strength) {
    this->rotationblurFilterParams.strength = _strength;
  }
  void SetAngle(const float _angle) {
    this->rotationblurFilterParams.angle = _angle;
  }

  void Destroy() override;
};
#endif
