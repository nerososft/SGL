#include "MinMaxFilter.h"

#include "core/config.h"

VkResult
MinMaxFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                    const std::vector<FilterImageInfo> &inputImageInfo,
                    const std::vector<FilterImageInfo> &outputImageInfo) {
  BasicFilterParams params;
  this->minmaxFilterParams.imageSize.width = inputImageInfo[0].width;
  ;
  this->minmaxFilterParams.imageSize.height = inputImageInfo[0].height;
  ;
  this->minmaxFilterParams.imageSize.channels = 4;
  this->minmaxFilterParams.imageSize.bytesPerLine =
      this->minmaxFilterParams.imageSize.width * 4;
  params.paramsSize = sizeof(MinMaxFilterParams);
  params.paramsData = &this->minmaxFilterParams;
  params.shaderPath = SHADER(minmax.comp.glsl.spv);

  return BasicFilter::Apply(gpuCtx, "MinMaxvalue", inputImageInfo[0].bufferSize,
                            inputImageInfo[0].storageBuffer,
                            outputImageInfo[0].storageBuffer, params,
                            (outputImageInfo[0].width + 31) / 32,
                            (outputImageInfo[0].height + 31) / 32, 1);
}

void MinMaxFilter::Destroy() { BasicFilter::Destroy(); }
