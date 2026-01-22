//
// Created by neo on 2026/1/22.
//

#ifndef SGL_TILEBASEDSCALEFILTER_H
#define SGL_TILEBASEDSCALEFILTER_H
#include "core/image/tile_filters/ITileBasedFilter.h"
#include "runtime/gpu/compute_graph/ComputeGraph.h"

#include <vulkan/vulkan_core.h>

struct TiledScaleFilterParams {
  TileBasedFilterParam imageSize;
  uint32_t targetWidth;
  uint32_t targetHeight;
  uint32_t interpType = 1;
};

class TileBasedScaleFilter : public ITileBasedFilter {
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

  TiledScaleFilterParams scaleFilterParams{};
  FilterParams filterParams{};

public:
  TileBasedScaleFilter() = default;

  ~TileBasedScaleFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx, int tileIdx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetTargetWidth(const uint32_t width) {
    this->scaleFilterParams.targetWidth = width;
  }
  void SetTargetHeight(const uint32_t height) {
    this->scaleFilterParams.targetHeight = height;
  }
  void SetInterpType(const uint32_t type) {
    this->scaleFilterParams.interpType = type;
  }

  void Destroy() override;
};

#endif // SGL_TILEBASEDSCALEFILTER_H
