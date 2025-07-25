#include "ZigzagFilter.h"

#include "runtime/config.h"

VkResult
ZigzagFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                    const std::vector<FilterImageInfo> &inputImageInfo,
                    const std::vector<FilterImageInfo> &outputImageInfo) {
  BasicFilterParams params;
  this->zigzagFilterParams.imageSize.width = inputImageInfo[0].width;
  ;
  this->zigzagFilterParams.imageSize.height = inputImageInfo[0].height;
  ;
  this->zigzagFilterParams.imageSize.channels = 4;
  this->zigzagFilterParams.imageSize.bytesPerLine =
      this->zigzagFilterParams.imageSize.width * 4;
  params.paramsSize = sizeof(ZigzagFilterParams);
  params.paramsData = &this->zigzagFilterParams;
  params.shaderPath = SHADER(zigzag.comp.glsl.spv);

  return BasicFilter::Apply(gpuCtx, "ZigZag", inputImageInfo[0].bufferSize,
                            inputImageInfo[0].storageBuffer,
                            outputImageInfo[0].storageBuffer, params,
                            (outputImageInfo[0].width + 31) / 32,
                            (outputImageInfo[0].height + 31) / 32, 1);
}

void ZigzagFilter::Destroy() { BasicFilter::Destroy(); }
