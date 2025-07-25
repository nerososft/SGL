//
// Created by neo on 2025/3/19.
//

#ifndef NES8BITMOSAIC_H
#define NES8BITMOSAIC_H
#include "engine/image/filters/BasicFilter.h"

struct NES8BitMosaicFilterParams {
  BasicFilterParam imageSize;
  uint32_t blockSize;
};

class NES8BitMosaicFilter final : public BasicFilter {
  NES8BitMosaicFilterParams nes8BitMosaicFilterParams{};

public:
  NES8BitMosaicFilter() = default;

  ~NES8BitMosaicFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetBlockSize(const uint32_t blockSize) {
    nes8BitMosaicFilterParams.blockSize = blockSize;
  }

  void Destroy() override;
};

#endif // NES8BITMOSAIC_H
