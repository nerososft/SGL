#include "RotationBlurFilter.h"

#include "effect_engine/config.h"


VkResult RotationBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                   const std::vector<FilterImageInfo> &inputImageInfo,
                                   const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->rotationblurFilterParams.imageSize.width = inputImageInfo[0].width;
    this->rotationblurFilterParams.imageSize.height = inputImageInfo[0].height;
    this->rotationblurFilterParams.imageSize.channels = 4;
    this->rotationblurFilterParams.imageSize.bytesPerLine = this->rotationblurFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(RotationBlurFilterParams);
    params.paramsData = &this->rotationblurFilterParams;
    params.shaderPath = SHADER(rotationblur.comp.glsl.spv);


    return BasicFilter::Apply(gpuCtx,
                              "rotationblur",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (outputImageInfo[0].width + 31) / 32,
                              (outputImageInfo[0].height + 31) / 32,
                              1);
}

void RotationBlurFilter::Destroy() {
    BasicFilter::Destroy();
}
