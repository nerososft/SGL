//
// Created by neo on 2026/1/20.
//

#include "TileBasedFilter.h"

VkResult
TileBasedFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx, int tileIdx,
                       const std::vector<FilterImageInfo> &inputImageInfo,
                       const std::vector<FilterImageInfo> &outputImageInfo) {

  return VK_SUCCESS;
}

void TileBasedFilter::Destroy() {

}