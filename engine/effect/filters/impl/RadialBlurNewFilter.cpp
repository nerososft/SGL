//
// Created by neo on 2025/3/13.
//

#include "RadialBlurNewFilter.h"

#include "core/config.h"

VkResult RadialBlurNewFilter::Apply(
    const std::shared_ptr<VkGPUContext> &gpuCtx,
    const std::vector<FilterImageInfo> &inputImageInfo,
    const std::vector<FilterImageInfo> &outputImageInfo) {
  BasicFilterParams params;
  this->radialBlurFilterParams.imageSize.width = inputImageInfo[0].width;
  this->radialBlurFilterParams.imageSize.height = inputImageInfo[0].height;
  this->radialBlurFilterParams.imageSize.channels = 4;
  this->radialBlurFilterParams.imageSize.bytesPerLine =
      this->radialBlurFilterParams.imageSize.width * 4;
  params.paramsSize = sizeof(RadialBlurNewFilterParams);
  params.paramsData = &this->radialBlurFilterParams;
  params.shaderPath = SHADER(radial_blur_new.comp.glsl.spv);
  return BasicFilter::Apply(
      gpuCtx, "RadialBlurNew", inputImageInfo[0].bufferSize,
      inputImageInfo[0].storageBuffer, outputImageInfo[0].storageBuffer, params,
      (outputImageInfo[0].width + 31) / 32,
      (outputImageInfo[0].height + 31) / 32, 1);
}

void RadialBlurNewFilter::Destroy() { BasicFilter::Destroy(); }
