//
// Created by 1234 on 2025/3/6.
//

#include "GrayFilter.h"

#include "effect_engine/config.h"

VkResult GrayFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           const VkDeviceSize bufferSize,
                           const uint32_t width,
                           const uint32_t height,
                           const VkBuffer inputBuffer,
                           const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->grayFilterParams.imageSize.width = width;
    this->grayFilterParams.imageSize.height = height;
    this->grayFilterParams.imageSize.channels = 4;
    this->grayFilterParams.imageSize.bytesPerLine = this->grayFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(GrayFilterParams);
    params.paramsData = &this->grayFilterParams;
    params.shaderPath = SHADER(gray.comp.glsl.spv);
    return DoApply(gpuCtx,
                   "Gray",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (width + 31) / 32,
                   (height + 31) / 32,
                   1);
}

void GrayFilter::Destroy() {
    BasicFilter::Destroy();
}
