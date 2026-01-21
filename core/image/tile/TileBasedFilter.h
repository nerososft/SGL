//
// Created by neo on 2026/1/20.
//

#ifndef SGL_TILEBASEDFILTER_H
#define SGL_TILEBASEDFILTER_H
#include "core/image/filters/IFilter.h"
#include "runtime/gpu/VkGPUContext.h"
#include "runtime/gpu/compute_graph/ComputeGraph.h"

#include <memory>
#include <vulkan/vulkan_core.h>

struct TileBasedFilterParam {
  uint32_t imageTotalWidth;
  uint32_t imageTotalHeight;
  uint32_t channels;
  uint32_t bytesPerLine;
  uint32_t tileHeight;
  uint32_t currentTileIndex;
};

struct FilterParams {
  size_t paramsSize;
  void *paramsData;
  std::string shaderPath;
};

class TileBasedFilter {
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

  TileBasedFilterParam tiledFilterParams{};
  FilterParams filterParams{};

public:
  TileBasedFilter() = default;

  virtual ~TileBasedFilter() = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx, int tileIdx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo);

  void Destroy();
};

#endif // SGL_TILEBASEDFILTER_H
