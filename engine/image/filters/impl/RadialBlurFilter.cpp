//
// Created by neo on 2025/3/13.
//

#include "RadialBlurFilter.h"

#include "core/config.h"

VkResult
RadialBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                        const std::vector<FilterImageInfo> &inputImageInfo,
                        const std::vector<FilterImageInfo> &outputImageInfo) {
  BasicFilterParams params;
  this->radialBlurFilterParams.imageSize.width = inputImageInfo[0].width;
  this->radialBlurFilterParams.imageSize.height = inputImageInfo[0].height;
  this->radialBlurFilterParams.imageSize.channels = 4;
  this->radialBlurFilterParams.imageSize.bytesPerLine =
      this->radialBlurFilterParams.imageSize.width * 4;
  params.paramsSize = sizeof(RadialBlurFilterParams);
  params.paramsData = &this->radialBlurFilterParams;
  params.shaderPath = SHADER(radial_blur.comp.glsl.spv);
  return BasicFilter::Apply(gpuCtx, "RadialBlur", inputImageInfo[0].bufferSize,
                            inputImageInfo[0].storageBuffer,
                            outputImageInfo[0].storageBuffer, params,
                            (outputImageInfo[0].width + 15) / 16,
                            (outputImageInfo[0].height + 15) / 16, 1);
}

void RadialBlurFilter::Destroy() { BasicFilter::Destroy(); }
