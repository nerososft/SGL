//
// Created by neo on 2025/3/13.
//

#ifndef RADIALBLURNEWFILTER_H
#define RADIALBLURNEWFILTER_H
#include "core/image/filters/BasicFilter.h"

struct RadialBlurNewFilterParams {
  BasicFilterParam imageSize;

  float blurRadius;
  int iteration;
  float xCenter;
  float yCenter;
};

class RadialBlurNewFilter final : public BasicFilter {
  RadialBlurNewFilterParams radialBlurFilterParams{};

public:
  RadialBlurNewFilter() = default;

  ~RadialBlurNewFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetCenter(const float xPos, const float yPos) {
    this->radialBlurFilterParams.xCenter = xPos;
    this->radialBlurFilterParams.yCenter = yPos;
  }

  void SetStrength(const int _strength) {
    this->radialBlurFilterParams.iteration = _strength;
  }

  void SetSharpness(const float _sharpness) {
    this->radialBlurFilterParams.blurRadius = _sharpness;
  }

  void Destroy() override;
};

#endif // RADIALBLURFILTER_H
