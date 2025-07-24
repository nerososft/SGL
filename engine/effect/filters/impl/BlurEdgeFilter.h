//
// Created by neo on 2025/3/18.
//

#ifndef BLUREDGE_H
#define BLUREDGE_H

#include "engine/effect/filters/BasicFilter.h"

struct BlurEdgeFilterParams {
  BasicFilterParam imageSize;
  int radius;
  int sigma;
  int k;
};

class BlurEdgeFilter final : public BasicFilter {
  BlurEdgeFilterParams blurEdgeFilterParams{};

public:
  BlurEdgeFilter() = default;

  ~BlurEdgeFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetRadius(const int radius) {
    this->blurEdgeFilterParams.radius = radius;
  }

  void SetSigma(const int sigma) { this->blurEdgeFilterParams.sigma = sigma; }
  void SetKernel(const int k) { this->blurEdgeFilterParams.k = k; }

  void Destroy() override;
};

#endif // PALETTEKNIFEFILTER_H
