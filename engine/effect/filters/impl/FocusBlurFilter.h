//
// Created by neo on 2025/3/13.
//

#ifndef FOCUSBLURFILTER_H
#define FOCUSBLURFILTER_H
#include "engine/effect/filters/BasicFilter.h"

struct FocusBlurFilterParams {
  BasicFilterParam imageSize;
  uint32_t centerX;
  uint32_t centerY;
  uint32_t blurRadius;
  uint32_t centerRadius;
  float sharpness;
};

class FocusBlurFilter final : public BasicFilter {
  FocusBlurFilterParams focusBlurFilterParams = {};

public:
  FocusBlurFilter() = default;

  ~FocusBlurFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetCenter(const uint32_t xPos, const uint32_t yPos) {
    this->focusBlurFilterParams.centerX = xPos;
    this->focusBlurFilterParams.centerY = yPos;
  }

  void SetBlurRadius(const uint32_t radius) {
    this->focusBlurFilterParams.blurRadius = radius;
  }

  void SetCenterRadius(const uint32_t centerRadius) {
    this->focusBlurFilterParams.centerRadius = centerRadius;
  }

  void SetSharpness(const float sharpness) {
    this->focusBlurFilterParams.sharpness = sharpness;
  }

  void Destroy() override;
};

#endif // FOCUSBLURFILTER_H
