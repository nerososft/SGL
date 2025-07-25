//
// Created by neo on 25-4-3.
//

#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H
#include <runtime/gpu/compute_graph/ComputeGraph.h>

#include "engine/image/filters/BasicFilter.h"
#include "engine/image/filters/IFilter.h"

struct MedianFilterParams {
  BasicFilterParam imageSize;
  uint32_t piece;
  uint32_t pieceCount;
  float radius;
};

class MedianFilter final : public IFilter {
  std::vector<MedianFilterParams> medianFilterParams;
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  float radius = 0.0f;

public:
  MedianFilter() = default;

  ~MedianFilter() override = default;

  std::shared_ptr<SubComputeGraph>
  CreateParallelSubGraph(size_t parallelIndex,
                         const std::shared_ptr<VkGPUContext> &gpuCtx,
                         VkBuffer inputBuffer, VkDeviceSize inputBufferSize,
                         VkBuffer outputBuffer, VkDeviceSize outputBufferSize);

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetRadius(const float radius) { this->radius = radius; }

  void Destroy() override;
};

#endif // MEDIANFILTER_H
