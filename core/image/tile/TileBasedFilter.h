//
// Created by neo on 2026/1/20.
//

#ifndef SGL_TILEBASEDFILTER_H
#define SGL_TILEBASEDFILTER_H
#include "core/image/filters/IFilter.h"
#include "runtime/gpu/VkGPUContext.h"

#include <memory>
#include <vulkan/vulkan_core.h>

class TileBasedFilter {
public:
  TileBasedFilter() = default;

  virtual ~TileBasedFilter() = default;

  virtual VkResult
  Apply(const std::shared_ptr<VkGPUContext> &gpuCtx, int tileIdx,
        const std::vector<FilterImageInfo> &inputImageInfo,
        const std::vector<FilterImageInfo> &outputImageInfo) = 0;

  virtual void Destroy() = 0;
};

#endif // SGL_TILEBASEDFILTER_H
