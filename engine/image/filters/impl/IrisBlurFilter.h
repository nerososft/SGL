#pragma once
#ifndef IRISBLURFILTER_H
#define IRISBLURFILTER_H
#include <vulkan/vulkan_core.h>

#include "engine/image/filters/BasicFilter.h"
#include "runtime/gpu/VkGPUContext.h"

struct IrisBlurFilterParams {
  BasicFilterParam imageSize;
  float x;
  float y;
  float a;
  float b;
  float ina;
  float inb;
  float angle;
};

class IrisBlurFilter final : public IFilter {
  IrisBlurFilterParams irisblurFilterParams{};
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

public:
  IrisBlurFilter() = default;

  ~IrisBlurFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetCenterX(const float _x) { this->irisblurFilterParams.x = _x; }
  void SetCenterY(const float _y) { this->irisblurFilterParams.y = _y; }
  void SetA(const float _a) { this->irisblurFilterParams.a = _a; }
  void SetB(const float _b) { this->irisblurFilterParams.b = _b; }
  void SetinA(const float _ina) { this->irisblurFilterParams.ina = _ina; }
  void SetinB(const float _inb) { this->irisblurFilterParams.inb = _inb; }
  void SetAngle(const float _angle) {
    this->irisblurFilterParams.angle = _angle;
  }

  void Destroy() override;
};
#endif
