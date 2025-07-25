//
// Created by 1234 on 2025/3/6.
//

#include "ThresholdSplitFilter.h"

#include "core/config.h"

VkResult ThresholdSplitFilter::Apply(
    const std::shared_ptr<VkGPUContext> &gpuCtx,
    const std::vector<FilterImageInfo> &inputImageInfo,
    const std::vector<FilterImageInfo> &outputImageInfo) {
  BasicFilterParams params;
  this->thresholdSplitFilterParams.imageSize.width = inputImageInfo[0].width;
  this->thresholdSplitFilterParams.imageSize.height = inputImageInfo[0].height;
  this->thresholdSplitFilterParams.imageSize.channels = 4;
  this->thresholdSplitFilterParams.imageSize.bytesPerLine =
      this->thresholdSplitFilterParams.imageSize.width * 4;
  params.paramsSize = sizeof(ThresholdSplitFilterParams);
  params.paramsData = &this->thresholdSplitFilterParams;
  params.shaderPath = SHADER(threshold_split.comp.glsl.spv);
  return BasicFilter::Apply(
      gpuCtx, "ThresholdSplit", inputImageInfo[0].bufferSize,
      inputImageInfo[0].storageBuffer, outputImageInfo[0].storageBuffer, params,
      (outputImageInfo[0].width + 31) / 32,
      (outputImageInfo[0].height + 31) / 32, 1);
}

void ThresholdSplitFilter::Destroy() { BasicFilter::Destroy(); }
