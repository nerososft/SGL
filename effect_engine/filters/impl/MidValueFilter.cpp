#include "MidValueFilter.h"

#include "effect_engine/config.h"

VkResult MidValueFilter::Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
    const VkDeviceSize bufferSize,
    const uint32_t width,
    const uint32_t height,
    const VkBuffer inputBuffer,
    const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->midvalueFilterParams.imageSize.width = width;
    this->midvalueFilterParams.imageSize.height = height;
    this->midvalueFilterParams.imageSize.channels = 4;
    this->midvalueFilterParams.imageSize.bytesPerLine = this->midvalueFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(MidValueFilterParams);
    params.paramsData = &this->midvalueFilterParams;
    params.shaderPath = SHADER(midvalue.comp.glsl.spv);
    return DoApply(gpuCtx,
        "MidValue",
        bufferSize,
        inputBuffer,
        outputBuffer,
        params,
        (width + 31) / 32,
        (height + 31) / 32,
        1);
}

void MidValueFilter::Destroy() {
    BasicFilter::Destroy();
}