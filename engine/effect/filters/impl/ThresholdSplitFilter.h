//
// Created by 1234 on 2025/3/6.
//

#ifndef THREADHOLDFILTER
#define THREADHOLDFILTER
#include <vulkan/vulkan_core.h>

#include "core/gpu/VkGPUContext.h"
#include "engine/effect/filters/BasicFilter.h"

struct ThresholdSplitFilterParams {
  BasicFilterParam imageSize;
  int bright;
};

class ThresholdSplitFilter final : public BasicFilter {
  ThresholdSplitFilterParams thresholdSplitFilterParams{};

public:
  ThresholdSplitFilter() = default;

  ~ThresholdSplitFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetBright(const int bright) {
    this->thresholdSplitFilterParams.bright = bright;
  }

  void Destroy() override;
};

#endif // THREADHOLDFILTER
