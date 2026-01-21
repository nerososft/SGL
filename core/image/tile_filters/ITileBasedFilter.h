//
// Created by neo on 2026/1/21.
//

#ifndef SGL_ITILEBASEDFILTER_H
#define SGL_ITILEBASEDFILTER_H

#include "core/image/filters/IFilter.h"

#include <cstdint>

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

class ITileBasedFilter {
public:
  ITileBasedFilter() = default;

  virtual ~ITileBasedFilter() = default;

  virtual VkResult
  Apply(const std::shared_ptr<VkGPUContext> &gpuCtx, int tileIdx,
        const std::vector<FilterImageInfo> &inputImageInfo,
        const std::vector<FilterImageInfo> &outputImageInfo) = 0;

  virtual void Destroy() = 0;
};

#endif // SGL_ITILEBASEDFILTER_H
