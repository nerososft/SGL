//
// Created by neo on 2026/1/23.
//

#ifndef SGL_TILEBASEDVERTICALBLURFILTER_H
#define SGL_TILEBASEDVERTICALBLURFILTER_H

#include "core/image/tile_filters/ITileBasedFilter.h"
#include "runtime/gpu/compute_graph/ComputeGraph.h"

#include <vulkan/vulkan_core.h>

struct TiledVerticalBlurFilterParams {
  TileBasedFilterParam imageSize;
  uint32_t radius;
};

class TileBasedVerticalBlurFilter : public ITileBasedFilter {
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

  TiledVerticalBlurFilterParams blurFilterParams{};
  FilterParams filterParams{};

public:
  TileBasedVerticalBlurFilter() = default;

  ~TileBasedVerticalBlurFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx, int tileIdx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetRadius(const uint32_t radius) {
    this->blurFilterParams.radius = radius;
  }

  void Destroy() override;
};

#endif // SGL_TILEBASEDVERTICALBLURFILTER_H
