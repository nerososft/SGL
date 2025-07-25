//
// Created by neo on 2025/3/11.
//

#ifndef GAUSSIANBLURFILTER_H
#define GAUSSIANBLURFILTER_H
#include "core/image/filters/BasicFilter.h"
#include "core/image/filters/IFilter.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/compute_graph/ComputeGraph.h"

#define MAX_RADIUS (256)

struct GaussianBlurFilterParams {
  BasicFilterParam imageSize;
  int radius;
};

class GaussianBlurFilter final : public IFilter {
  GaussianBlurFilterParams blurFilterParams{};
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

private:
  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;

  static std::vector<float> CalculateWeights();

  std::shared_ptr<VkGPUBuffer> weightBuffer = nullptr;

public:
  GaussianBlurFilter() = default;

  ~GaussianBlurFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetRadius(const int radius) { this->blurFilterParams.radius = radius; }

  void Destroy() override;
};

#endif // GAUSSIANBLURFILTER_H
