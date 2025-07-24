#pragma once
//
// Created by 1234 on 2025/3/6.
//

#ifndef ACCENTEDEDGEFILTER_H
#define ACCENTEDEDGEFILTER_H
#include <vulkan/vulkan_core.h>

#include "core/gpu/VkGPUContext.h"
#include "engine/effect/filters/BasicFilter.h"
#include <core/gpu/VkGPUBuffer.h>

struct AccentedEdgeFilterParams {
  BasicFilterParam imageSize;
  int type;
};

class AccentedEdgeFilter final : public IFilter {
  AccentedEdgeFilterParams accentedEdgeFilterParams{};
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
  std::shared_ptr<VkGPUBuffer> sobelxBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> sobelyBuffer = nullptr;
  int s_size = 0;
  int *sobelx = nullptr;
  int *sobely = nullptr;

public:
  AccentedEdgeFilter() = default;

  ~AccentedEdgeFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetSobelx(int *_sobelx, const int size) {
    this->sobelx = _sobelx;
    this->s_size = size;
  }

  void SetSobely(int *_sobely, const int size) {
    this->sobely = _sobely;
    // this->s_size = size;
  }

  void SetType(int type) { this->accentedEdgeFilterParams.type = type; }

  void Destroy() override;
};

#endif // GRAYFILTER_H
