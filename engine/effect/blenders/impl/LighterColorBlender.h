//
// Created by Jun Li on 2025/3/14.
//

#ifndef LIGHTERCOLORBLENDER_H
#define LIGHTERCOLORBLENDER_H
#include "engine/effect/blenders/BaseBlender.h"

struct LighterColorBlenderParams {
  BasicBlenderParam imageSize;
  float brightnessThreshold; // 亮度比较阈值（0.0-1.0）
  float blendFactor;         // 混合因子（透明度）
};

class LighterColorBlender final : public BaseBlender {
  LighterColorBlenderParams lighterColorBlenderParams{};

public:
  LighterColorBlender() = default;

  ~LighterColorBlender() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 BlendImageInfo baseImageInfo, BlendImageInfo blendImageInfo,
                 VkBuffer outputBuffer) override;

  void SetBrightnessThreshold(const float threshold) {
    this->lighterColorBlenderParams.brightnessThreshold = threshold;
  }

  void SetBlendFactor(const float factor) {
    this->lighterColorBlenderParams.blendFactor = factor;
  }

  void Destroy() override;
};

#endif // LIGHTERCOLORBLENDER_H
