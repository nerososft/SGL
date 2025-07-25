#pragma once
#ifndef MIDVALUEFILTER_H
#define MIDVALUEFILTER_H
#include <vulkan/vulkan_core.h>

#include "core/gpu/VkGPUContext.h"
#include "engine/image/filters/BasicFilter.h"

struct MidValueFilterParams {
  BasicFilterParam imageSize;
  float radius;
  float threshold;
};

class MidValueFilter final : public BasicFilter {
  MidValueFilterParams midvalueFilterParams{};

public:
  MidValueFilter() = default;

  ~MidValueFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetRadius(const float radius) {
    this->midvalueFilterParams.radius = radius;
  }
  void SetThreshold(const float threshold) {
    this->midvalueFilterParams.threshold = threshold;
  }

  void Destroy() override;
};
#endif
