#pragma once
#ifndef PATHBLURFILTER_H
#define PATHBLURFILTER_H
#include <vulkan/vulkan_core.h>

#include "core/image/filters/BasicFilter.h"
#include "runtime/gpu/VkGPUContext.h"
#include <runtime/gpu/VkGPUBuffer.h>

struct pathBlurFilterParams {
  BasicFilterParam imageSize;
  int amount;
  int num;
};

class pathBlurFilter final : public IFilter {
  pathBlurFilterParams pathblurFilterParams{};
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
  std::shared_ptr<VkGPUBuffer> vecBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> startposBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> endposBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> startvecBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> endvecBuffer = nullptr;
  float *vec = nullptr;
  float *startpos = nullptr;
  float *endpos = nullptr;
  float *startvec = nullptr;
  float *endvec = nullptr;
  int k_size = 0;
  // int num = 0;

public:
  pathBlurFilter() = default;

  ~pathBlurFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetK1(float *vec, const int size) {
    this->vec = vec;
    this->k_size = size;
  }

  void SetK(const int size) { this->k_size = size; }

  void SetStartPos(float *_startpos, const int num) {
    this->startpos = _startpos;
    this->pathblurFilterParams.num = num;
  }

  void SetEndPos(float *_endpos, int num) { this->endpos = _endpos; }

  void SetStartVec(float *_startvec, int num) { this->startvec = _startvec; }

  void SetEndVec(float *_endvec, int num) { this->endvec = _endvec; }

  void SetAmount(const int amount) {
    this->pathblurFilterParams.amount = amount;
  }

  void Destroy() override;
};
#endif
