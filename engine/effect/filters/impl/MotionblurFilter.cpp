#include "MotionblurFilter.h"

#include "core/config.h"

VkResult MotionBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                 const std::vector<FilterImageInfo> &inputImageInfo,
                                 const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->motionBlurFilterParams.imageSize.width = inputImageInfo[0].width;;
    this->motionBlurFilterParams.imageSize.height = inputImageInfo[0].height;;
    this->motionBlurFilterParams.imageSize.channels = 4;
    this->motionBlurFilterParams.imageSize.bytesPerLine = this->motionBlurFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(MotionBlurFilterParams);
    params.paramsData = &this->motionBlurFilterParams;
    params.shaderPath = SHADER(motionblur.comp.glsl.spv);

    return BasicFilter::Apply(gpuCtx,
                              "MotionBlur",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (outputImageInfo[0].width + 31) / 32,
                              (outputImageInfo[0].height + 31) / 32,
                              1);
}

void MotionBlurFilter::Destroy() {
    BasicFilter::Destroy();
}
