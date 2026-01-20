//
// Created by neo on 2026/1/20.
//

#ifndef SGL_TILEBASEDFILTER_H
#define SGL_TILEBASEDFILTER_H
#include "runtime/gpu/compute_graph/ComputeGraph.h"

struct TileBasedFilterParam {
  uint32_t width;
  uint32_t height;
  uint32_t channels;
  uint32_t bytesPerLine;
};

struct TileBasedFilterParams {
  size_t paramsSize;
  void *paramsData;
  std::string shaderPath;
};

class TileBasedFilter : public IFilter {
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

public:
  TileBasedFilter() = default;

  ~TileBasedFilter() override = default;

  virtual VkResult
  Apply(const std::shared_ptr<VkGPUContext> &gpuCtx, int tileIdx,
        const std::vector<FilterImageInfo> &inputImageInfo,
        const std::vector<FilterImageInfo> &outputImageInfo) = 0;

  void Destroy() override;
};
#endif // SGL_TILEBASEDFILTER_H
