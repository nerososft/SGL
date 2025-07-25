//
// Created by 1234 on 2025/3/6.
//

#ifndef BLACKWHITEFILTER
#define BLACKWHITEFILTER
#include <vulkan/vulkan_core.h>

#include "core/image/filters/BasicFilter.h"
#include "runtime/gpu/VkGPUContext.h"
#include <runtime/gpu/VkGPUBuffer.h>

struct BlackWhiteFilterParams {
  BasicFilterParam imageSize;
};

class BlackWhiteFilter final : public BasicFilter {
  BlackWhiteFilterParams wFilterParams{};
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<VkGPUBuffer> weightBuffer = nullptr;

  float *weight = nullptr;
  int wSize = 0;

public:
  BlackWhiteFilter() = default;

  ~BlackWhiteFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetWeight(float *weight, const int size) {
    this->weight = weight;
    this->wSize = size;
  }

  void Destroy() override;
};

#endif // GRAYFILTER_H
