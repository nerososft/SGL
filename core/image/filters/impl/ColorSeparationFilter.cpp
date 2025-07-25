//
// Created by neo on 2025/3/12.
//

#include "ColorSeparationFilter.h"

#include "ColorPencilSketchFilter.h"
#include "runtime/config.h"

VkResult ColorSeparationFilter::Apply(
    const std::shared_ptr<VkGPUContext> &gpuCtx,
    const std::vector<FilterImageInfo> &inputImageInfo,
    const std::vector<FilterImageInfo> &outputImageInfo) {
  BasicFilterParams params;
  this->colorSeparationFilterParams.imageSize.width = inputImageInfo[0].width;
  this->colorSeparationFilterParams.imageSize.height = inputImageInfo[0].height;
  this->colorSeparationFilterParams.imageSize.channels = 4;
  this->colorSeparationFilterParams.imageSize.bytesPerLine =
      this->colorSeparationFilterParams.imageSize.width * 4;
  params.paramsSize = sizeof(ColorSeparationFilterParams);
  params.paramsData = &this->colorSeparationFilterParams;
  params.shaderPath = SHADER(color_separation.comp.glsl.spv);
  return BasicFilter::Apply(
      gpuCtx, "ColorSeparation", inputImageInfo[0].bufferSize,
      inputImageInfo[0].storageBuffer, outputImageInfo[0].storageBuffer, params,
      (outputImageInfo[0].width + 15) / 16,
      (outputImageInfo[0].height + 15) / 16, 1);
}

void ColorSeparationFilter::Destroy() { BasicFilter::Destroy(); }
