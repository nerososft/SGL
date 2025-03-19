//
// Created by neo on 2025/3/18.
//

#include "VibranceFilter.h"

#include "effect_engine/config.h"

VkResult VibranceFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                               const VkDeviceSize bufferSize,
                               const uint32_t width,
                               const uint32_t height,
                               const VkBuffer inputBuffer,
                               const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->vibranceFilterParams.imageSize.width = width;
    this->vibranceFilterParams.imageSize.height = height;
    this->vibranceFilterParams.imageSize.channels = 4;
    this->vibranceFilterParams.imageSize.bytesPerLine = this->vibranceFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(VibranceFilterParams);
    params.paramsData = &this->vibranceFilterParams;
    params.shaderPath = SHADER(vibrance.comp.glsl.spv);
    return DoApply(gpuCtx,
                   "VibranceFilter",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (width + 31) / 32,
                   (height + 31) / 32,
                   1);
}

void VibranceFilter::Destroy() {
    BasicFilter::Destroy();
}
