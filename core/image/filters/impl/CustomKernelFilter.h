//
// Created by 1234 on 2025/3/6.
//

#ifndef KFILTER_H
#define KFILTER_H
#include <vulkan/vulkan_core.h>

#include "core/image/filters/BasicFilter.h"
#include "runtime/gpu/VkGPUContext.h"
#include <runtime/gpu/VkGPUBuffer.h>

struct customKernelFilterParams {
  BasicFilterParam imageSize;
  int radius;
  int scale;
  int offset;
};

class CustomKernelFilter final : public IFilter {
  customKernelFilterParams kFilterParams{};
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
  std::shared_ptr<VkGPUBuffer> kBuffer = nullptr;
  int *k = nullptr;
  int k_size = 0;

public:
  CustomKernelFilter() = default;

  ~CustomKernelFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetK(int *k, const int size) {
    this->k = k;
    this->k_size = size;
  }

  void SetRadius(const int radius) { this->kFilterParams.radius = radius; }
  void SetScale(const int scale) { this->kFilterParams.scale = scale; }
  void SetOffset(const int offset) { this->kFilterParams.offset = offset; }

  void Destroy() override;
};

#endif // GRAYFILTER_H
