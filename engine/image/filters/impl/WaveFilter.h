#pragma once
#ifndef WAVEFILTER_H
#define WAVEFILTER_H
#include <vulkan/vulkan_core.h>

#include "engine/image/filters/BasicFilter.h"
#include "runtime/gpu/VkGPUContext.h"

struct WaveFilterParams {
  BasicFilterParam imageSize;
  int wavelength;
  int amplitude;
  int x_pro;
  int y_pro;
  int type;
  int method;
};

class WaveFilter final : public BasicFilter {
  WaveFilterParams waveFilterParams{};

public:
  WaveFilter() = default;

  ~WaveFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetWavelength(const int wavelength) {
    this->waveFilterParams.wavelength = wavelength;
  }
  void SetAmplitude(const int amplitude) {
    this->waveFilterParams.amplitude = amplitude;
  }

  void SetPro(const int x_pro, const int y_pro) {
    this->waveFilterParams.x_pro = x_pro;
    this->waveFilterParams.y_pro = y_pro;
  }

  void SetType(int type) { this->waveFilterParams.type = type; }
  void SetMethod(int method) { this->waveFilterParams.method = method; }

  void Destroy() override;
};
#endif
