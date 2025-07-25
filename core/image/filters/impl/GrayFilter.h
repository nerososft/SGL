//
// Created by 1234 on 2025/3/6.
//

#ifndef GRAYFILTER_H
#define GRAYFILTER_H
#include <vulkan/vulkan_core.h>

#include "core/image/filters/BasicFilter.h"
#include "runtime/gpu/VkGPUContext.h"

struct GrayFilterParams {
  BasicFilterParam imageSize;
  float redFactor;
  float greenFactor;
  float blueFactor;
};

class GrayFilter final : public BasicFilter {
  GrayFilterParams grayFilterParams{};

public:
  GrayFilter() = default;

  ~GrayFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetRedFactor(const float redFactor) {
    this->grayFilterParams.redFactor = redFactor;
  }
  void SetGreenFactor(const float greenFactor) {
    this->grayFilterParams.greenFactor = greenFactor;
  }
  void SetBlueFactor(const float blueFactor) {
    this->grayFilterParams.blueFactor = blueFactor;
  }

  void Destroy() override;
};

#endif // GRAYFILTER_H
