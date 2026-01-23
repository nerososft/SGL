//
// Created by neo on 2026/1/22.
//

#ifndef SGL_TILEBASEDGAUSSIANBLURFILTER_H
#define SGL_TILEBASEDGAUSSIANBLURFILTER_H

#include "../ITileBasedFilter.h"
#include "TileBasedScaleFilter.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/VkGPUContext.h"
#include "runtime/gpu/compute_graph/ComputeGraph.h"

#include <memory>
#include <vulkan/vulkan_core.h>

struct GaussianBlurFilterParams {
  TileBasedFilterParam imageSize;
  int radius;
};

class TileBasedGaussianBlurFilter : public ITileBasedFilter {
  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

  std::vector<FilterImageInfo> hBlurImageInfo;

  GaussianBlurFilterParams gaussianFilterParams{};
  FilterParams filterParams{};

  std::shared_ptr<VkGPUBuffer> vBlurOutputBlurBuffer = nullptr;

  std::shared_ptr<IComputeGraphNode>
  CreateTileBasedBlurNode(const std::vector<FilterImageInfo> &inputImageInfo,
                          const std::string &shaderPath, VkBuffer buffer,
                          VkDeviceSize bufferSize);

public:
  TileBasedGaussianBlurFilter() = default;

  ~TileBasedGaussianBlurFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx, int tileIdx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetRadius(const int radius) {
    this->gaussianFilterParams.radius = radius;
  }

  void Destroy() override;
};

#endif // SGL_TILEBASEDGAUSSIANBLURFILTER_H
