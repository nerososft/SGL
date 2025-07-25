#pragma once
#ifndef MOTIONBLURFILTER_H
#define MOTIONBLURFILTER_H
#include <vulkan/vulkan_core.h>

#include "core/gpu/VkGPUContext.h"
#include "engine/image/filters/BasicFilter.h"

struct MotionBlurFilterParams {
  BasicFilterParam imageSize;
  int distance;
  int angle;
  float proportion;
};

class MotionBlurFilter final : public BasicFilter {
  MotionBlurFilterParams motionBlurFilterParams{};

public:
  MotionBlurFilter() = default;

  ~MotionBlurFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetDistance(const int distance) {
    this->motionBlurFilterParams.distance = distance;
  }
  void SetAngle(const int angle) { this->motionBlurFilterParams.angle = angle; }
  void SetPro(const float proportion) {
    this->motionBlurFilterParams.proportion = proportion;
  }

  void Destroy() override;
};
#endif
