//
// Created by neo on 2025/3/14.
//

#ifndef MULTIPLYBLENDER_H
#define MULTIPLYBLENDER_H
#include "core/image/blenders/BaseBlender.h"

struct MultiplyBlenderParams {
  BasicBlenderParam imageSize;
};

class MultiplyBlender final : public BaseBlender {
  MultiplyBlenderParams multiplyBlenderParams{};

public:
  MultiplyBlender() = default;

  ~MultiplyBlender() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 BlendImageInfo baseImageInfo, BlendImageInfo blendImageInfo,
                 VkBuffer outputBuffer) override;

  void Destroy() override;
};

#endif // MULTIPLYBLENDER_H
