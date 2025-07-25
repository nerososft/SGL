//
// Created by neo on 25-7-8.
//

#ifndef RGBTOYUVFILTER_H
#define RGBTOYUVFILTER_H
#include <engine/image/filters/BasicFilter.h>

struct RgbToYuvFilterParams {
  BasicFilterParam imageSize;
  uint32_t outputWidthStride;
  uint32_t outputHeightStride;
  uint32_t format;
};

class RgbToYuvFilter final : public BasicFilter {
  RgbToYuvFilterParams rgbToYuvFilterParams{};

  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

public:
  RgbToYuvFilter() = default;

  ~RgbToYuvFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetOutputWidthStride(const uint32_t inputWidthStride) {
    this->rgbToYuvFilterParams.outputWidthStride = inputWidthStride;
  }

  void SetOutputHeightStride(const uint32_t inputHeightStride) {
    this->rgbToYuvFilterParams.outputHeightStride = inputHeightStride;
  }

  void SetFormat(const uint32_t format) {
    this->rgbToYuvFilterParams.format = format;
  }

  void Destroy() override;
};

#endif // RGBTOYUVFILTER_H
