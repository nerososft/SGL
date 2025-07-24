//
// Created by neo on 2025/3/13.
//

#ifndef SCALEFILTER_H
#define SCALEFILTER_H
#include "engine/effect/filters/BasicFilter.h"

struct ScaleFilterParams {
  BasicFilterParam imageSize;
  uint32_t targetWidth;
  uint32_t targetHeight;
  uint32_t interpType = 1;
};

class ScaleFilter final : public BasicFilter {
  ScaleFilterParams scaleFilterParams{};
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;

public:
  ScaleFilter() = default;

  ~ScaleFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void Destroy() override;

  void SetTargetWidth(const uint32_t width) {
    this->scaleFilterParams.targetWidth = width;
  }
  void SetTargetHeight(const uint32_t height) {
    this->scaleFilterParams.targetHeight = height;
  }
  void SetInterpType(const uint32_t type) {
    this->scaleFilterParams.interpType = type;
  }
};

#endif // SCALEFILTER_H
