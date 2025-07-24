#pragma once
#ifndef MINMAXFILTER_H
#define MINMAXFILTER_H
#include <vulkan/vulkan_core.h>

#include "core/gpu/VkGPUContext.h"
#include "engine/effect/filters/BasicFilter.h"

struct MinMaxFilterParams {
  BasicFilterParam imageSize;
  int radius;
  int type;
};

class MinMaxFilter final : public BasicFilter {
  MinMaxFilterParams minmaxFilterParams{};

public:
  MinMaxFilter() = default;

  ~MinMaxFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetRadius(const int radius) { this->minmaxFilterParams.radius = radius; }

  void SetType(const int type) { this->minmaxFilterParams.type = type; }

  void Destroy() override;
};
#endif
