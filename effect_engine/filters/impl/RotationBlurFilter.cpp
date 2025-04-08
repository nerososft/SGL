#include "RotationBlurFilter.h"

#include "effect_engine/config.h"

VkResult RotationBlurFilter::Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
    const VkDeviceSize bufferSize,
    const uint32_t width,
    const uint32_t height,
    const VkBuffer inputBuffer,
    const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->rotationblurFilterParams.imageSize.width = width;
    this->rotationblurFilterParams.imageSize.height = height;
    this->rotationblurFilterParams.imageSize.channels = 4;
    this->rotationblurFilterParams.imageSize.bytesPerLine = this->rotationblurFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(RotationBlurFilterParams);
    params.paramsData = &this->rotationblurFilterParams;
    params.shaderPath = SHADER(rotationblur.comp.glsl.spv);
    return DoApply(gpuCtx,
        "rotationblur",
        bufferSize,
        inputBuffer,
        outputBuffer,
        params,
        (width + 31) / 32,
        (height + 31) / 32,
        1);
}

void RotationBlurFilter::Destroy() {
    BasicFilter::Destroy();
}