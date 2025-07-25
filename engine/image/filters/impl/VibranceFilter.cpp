//
// Created by neo on 2025/3/18.
//

#include "VibranceFilter.h"

#include "runtime/config.h"

VkResult
VibranceFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                      const std::vector<FilterImageInfo> &inputImageInfo,
                      const std::vector<FilterImageInfo> &outputImageInfo) {
  BasicFilterParams params;
  this->vibranceFilterParams.imageSize.width = inputImageInfo[0].width;
  this->vibranceFilterParams.imageSize.height = inputImageInfo[0].height;
  this->vibranceFilterParams.imageSize.channels = 4;
  this->vibranceFilterParams.imageSize.bytesPerLine =
      this->vibranceFilterParams.imageSize.width * 4;
  params.paramsSize = sizeof(VibranceFilterParams);
  params.paramsData = &this->vibranceFilterParams;
  params.shaderPath = SHADER(vibrance.comp.glsl.spv);
  return BasicFilter::Apply(
      gpuCtx, "VibranceFilter", inputImageInfo[0].bufferSize,
      inputImageInfo[0].storageBuffer, outputImageInfo[0].storageBuffer, params,
      (outputImageInfo[0].width + 31) / 32,
      (outputImageInfo[0].height + 31) / 32, 1);
}

void VibranceFilter::Destroy() { BasicFilter::Destroy(); }
