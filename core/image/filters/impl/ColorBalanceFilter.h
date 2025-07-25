//
// Created by 1234 on 2025/3/6.
//

#ifndef COLORBALANCEFILTER_H
#define COLORBALANCEFILTER_H
#include <vulkan/vulkan_core.h>

#include "core/image/filters/BasicFilter.h"
#include "runtime/gpu/VkGPUContext.h"
#include <runtime/gpu/VkGPUBuffer.h>

struct colorBalanceFilterParams {
  BasicFilterParam imageSize;
  int preserve_luminosity;
};

class ColorBalanceFilter final : public IFilter {
  colorBalanceFilterParams bFilterParams{};
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
  std::shared_ptr<VkGPUBuffer> adjustPBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> PBuffer = nullptr;
  int *P = nullptr;
  float *adjustP = nullptr;
  int pSize = 0;
  int adjustPSize = 0;

public:
  ColorBalanceFilter() = default;

  ~ColorBalanceFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetP(int *p, const int size) {
    this->P = p;
    this->pSize = size;
  }

  void SetAdjustP(float *adjustP, const int size) {
    this->adjustP = adjustP;
    this->adjustPSize = size;
  }

  void SetL(const int l) { bFilterParams.preserve_luminosity = l; }

  void Destroy() override;
};

#endif // GRAYFILTER_H
