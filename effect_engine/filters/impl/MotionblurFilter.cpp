#include "MotionblurFilter.h"

#include "effect_engine/config.h"

VkResult MotionblurFilter::Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
    const std::vector<FilterImageInfo>& inputImageInfo,
    const std::vector<FilterImageInfo>& outputImageInfo) {
    BasicFilterParams params;
    this->motionblurFilterParams.imageSize.width = inputImageInfo[0].width;;
    this->motionblurFilterParams.imageSize.height = inputImageInfo[0].height;;
    this->motionblurFilterParams.imageSize.channels = 4;
    this->motionblurFilterParams.imageSize.bytesPerLine = this->motionblurFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(MotionblurFilterParams);
    params.paramsData = &this->motionblurFilterParams;
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

void MotionblurFilter::Destroy() {
    BasicFilter::Destroy();
}
