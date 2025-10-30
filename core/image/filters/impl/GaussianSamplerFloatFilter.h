//
// Created by neo on 2025/3/13.
//

#ifndef GAUSSIAN_SAMPLER_FLOAT_FILTER_H
#define GAUSSIAN_SAMPLER_FLOAT_FILTER_H
#include "core/image/filters/BasicFilter.h"

struct GaussianSamplerFilterParams {
  BasicFilterParam imageSize;
  float scale;
  float sigmaScale;
};

class GaussianSamplerFloatFilter final : public BasicFilter {
  GaussianSamplerFilterParams gaussianSamplerFilterParams{};
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;

public:
  GaussianSamplerFloatFilter() = default;

  ~GaussianSamplerFloatFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void Destroy() override;

  void SetScale(const float scale) {
    this->gaussianSamplerFilterParams.scale = scale;
  }
  void SetSigmaScale(const float sigmaScale) {
    this->gaussianSamplerFilterParams.sigmaScale = sigmaScale;
  }
};

#endif // GAUSSIAN_SAMPLER_FLOAT_FILTER_H
