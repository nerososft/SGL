#include "PinchFilter.h"

#include "runtime/config.h"

VkResult
PinchFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) {
  BasicFilterParams params;
  this->pinchFilterParams.imageSize.width = inputImageInfo[0].width;
  ;
  this->pinchFilterParams.imageSize.height = inputImageInfo[0].height;
  ;
  this->pinchFilterParams.imageSize.channels = 4;
  this->pinchFilterParams.imageSize.bytesPerLine =
      this->pinchFilterParams.imageSize.width * 4;
  params.paramsSize = sizeof(PinchFilterParams);
  params.paramsData = &this->pinchFilterParams;
  params.shaderPath = SHADER(pinch.comp.glsl.spv);

  return BasicFilter::Apply(gpuCtx, "Pinch", inputImageInfo[0].bufferSize,
                            inputImageInfo[0].storageBuffer,
                            outputImageInfo[0].storageBuffer, params,
                            (outputImageInfo[0].width + 31) / 32,
                            (outputImageInfo[0].height + 31) / 32, 1);
}

void PinchFilter::Destroy() { BasicFilter::Destroy(); }
