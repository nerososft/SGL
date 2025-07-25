#pragma once
#ifndef CRYSTALLIZEFILTER_H
#define CRYSTALLIZEFILTER_H
#include <vulkan/vulkan_core.h>

#include "core/gpu/VkGPUContext.h"
#include "engine/image/filters/BasicFilter.h"
#include <core/gpu/VkGPUBuffer.h>

struct CrystallizeFilterParams {
  BasicFilterParam imageSize;
  int n;
};

class CrystallizeFilter final : public IFilter {
  CrystallizeFilterParams crystallizeFilterParams{};
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
  std::shared_ptr<VkGPUBuffer> posxBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> posyBuffer = nullptr;
  float *posx = nullptr;
  float *posy = nullptr;
  int k_size = 0;

public:
  CrystallizeFilter() = default;

  ~CrystallizeFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;
  ;

  void SetPos(float *posx, float *posy, const int size) {
    this->posx = posx;
    this->posy = posy;
    this->k_size = size;
  }

  void SetN(const int n) { this->crystallizeFilterParams.n = n; }

  void Destroy() override;
};
#endif
