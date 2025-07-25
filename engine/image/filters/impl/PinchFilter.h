#pragma once
#ifndef PINCHFILTER_H
#define PINCHFILTER_H
#include <vulkan/vulkan_core.h>

#include "core/gpu/VkGPUContext.h"
#include "engine/image/filters/BasicFilter.h"

struct PinchFilterParams {
  BasicFilterParam imageSize;
  int amount;
};

class PinchFilter final : public BasicFilter {
  PinchFilterParams pinchFilterParams{};

public:
  PinchFilter() = default;

  ~PinchFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetAmount(const int amount) { this->pinchFilterParams.amount = amount; }

  void Destroy() override;
};
#endif
