//
// Created by neo on 2025/3/14.
//

#ifndef OVERLAYBLENDER_H
#define OVERLAYBLENDER_H
#include "core/image/blenders/BaseBlender.h"

struct OverlayBlenderParams {
  BasicBlenderParam imageSize;
};

class OverlayBlender final : public BaseBlender {
  OverlayBlenderParams overlayBlenderParams{};

public:
  OverlayBlender() = default;

  ~OverlayBlender() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 BlendImageInfo baseImageInfo, BlendImageInfo blendImageInfo,
                 VkBuffer outputBuffer) override;

  void Destroy() override;
};

#endif // OVERLAYBLENDER_H
