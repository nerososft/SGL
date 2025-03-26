//
// Created by 1234 on 2025/3/6.
//

#include "HueEqualFilter.h"

#include "effect_engine/config.h"

VkResult HueEqualFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           const VkDeviceSize bufferSize,
                           const uint32_t width,
                           const uint32_t height,
                           const VkBuffer inputBuffer,
                           const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->hueFilterParams.imageSize.width = width;
    this->hueFilterParams.imageSize.height = height;
    this->hueFilterParams.imageSize.channels = 4;
    this->hueFilterParams.imageSize.bytesPerLine = this->hueFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(HueEqualFilterParams);
    params.paramsData = &this->hueFilterParams;
    params.shaderPath = SHADER(hue_equal.comp.glsl.spv);
    return DoApply(gpuCtx,
                   "hueEqual",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (width + 31) / 32,
                   (height + 31) / 32,
                   1);
}

void HueEqualFilter::Destroy() {
    BasicFilter::Destroy();
}
