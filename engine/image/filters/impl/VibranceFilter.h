//
// Created by neo on 2025/3/18.
//

#ifndef VIBRANCEFILTER_H
#define VIBRANCEFILTER_H
#include "engine/image/filters/BasicFilter.h"

struct VibranceFilterParams {
  BasicFilterParam imageSize;
  float vibrance;
  float saturation;
};

class VibranceFilter final : public BasicFilter {
  VibranceFilterParams vibranceFilterParams{};

public:
  VibranceFilter() = default;

  ~VibranceFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetVibrance(const float vibrance) {
    this->vibranceFilterParams.vibrance = vibrance;
  }

  void SetSaturation(const float saturation) {
    this->vibranceFilterParams.saturation = saturation;
  }

  void Destroy() override;
};

#endif // VIBRANCEFILTER_H
