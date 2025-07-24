#pragma once
#ifndef POLARCOORDINATESFILTER_H
#define POLARCOORDINATESFILTER_H
#include <vulkan/vulkan_core.h>

#include "core/gpu/VkGPUContext.h"
#include "engine/effect/filters/BasicFilter.h"

struct PolarCoordinatesFilterParams {
  BasicFilterParam imageSize;
  int type;
};

class PolarCoordinatesFilter final : public BasicFilter {
  PolarCoordinatesFilterParams polarcoordinatesFilterParams{};

public:
  PolarCoordinatesFilter() = default;

  ~PolarCoordinatesFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetType(const int type) {
    this->polarcoordinatesFilterParams.type = type;
  }

  void Destroy() override;
};
#endif
