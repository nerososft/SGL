//
// Created by neo on 2026/1/20.
//

#ifndef SGL_TILEBASEDFILTER_H
#define SGL_TILEBASEDFILTER_H

#include "../ITileBasedFilter.h"
#include "runtime/gpu/VkGPUContext.h"
#include "runtime/gpu/compute_graph/ComputeGraph.h"

#include <memory>
#include <vulkan/vulkan_core.h>

struct GrayFilterParams {
  TileBasedFilterParam imageSize;
  float redFactor;
  float greenFactor;
  float blueFactor;
};

class TileBasedGrayFilter : public ITileBasedFilter {
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

  GrayFilterParams grayFilterParams{};
  FilterParams filterParams{};

public:
  TileBasedGrayFilter() = default;

  ~TileBasedGrayFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx, int tileIdx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetRedFactor(const float redFactor) {
    this->grayFilterParams.redFactor = redFactor;
  }
  void SetGreenFactor(const float greenFactor) {
    this->grayFilterParams.greenFactor = greenFactor;
  }
  void SetBlueFactor(const float blueFactor) {
    this->grayFilterParams.blueFactor = blueFactor;
  }

  void Destroy() override;
};

#endif // SGL_TILEBASEDFILTER_H
