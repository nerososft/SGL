//
// Created by 1234 on 2025/3/6.
//

#include "GrayFilter.h"

#include "runtime/config.h"

VkResult
GrayFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                  const std::vector<FilterImageInfo> &inputImageInfo,
                  const std::vector<FilterImageInfo> &outputImageInfo) {
  BasicFilterParams params;
  this->grayFilterParams.imageSize.width = inputImageInfo[0].width;
  this->grayFilterParams.imageSize.height = inputImageInfo[0].height;
  this->grayFilterParams.imageSize.channels = 4;
  this->grayFilterParams.imageSize.bytesPerLine =
      this->grayFilterParams.imageSize.width * 4;
  params.paramsSize = sizeof(GrayFilterParams);
  params.paramsData = &this->grayFilterParams;
  params.shaderPath = SHADER(gray.comp.glsl.spv);
  return BasicFilter::Apply(gpuCtx, "Gray", inputImageInfo[0].bufferSize,
                            inputImageInfo[0].storageBuffer,
                            outputImageInfo[0].storageBuffer, params,
                            (outputImageInfo[0].width + 31) / 32,
                            (outputImageInfo[0].height + 31) / 32, 1);
}

void GrayFilter::Destroy() { BasicFilter::Destroy(); }
