//
// Created by 1234 on 2025/3/6.
//

#include "ChinesePaintFilter.h"

#include "effect_engine/config.h"

VkResult ChinesePaintFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                   const VkDeviceSize bufferSize,
                                   const uint32_t width,
                                   const uint32_t height,
                                   const VkBuffer inputBuffer,
                                   const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->chinesePaintFilterParams.imageSize.width = width;
    this->chinesePaintFilterParams.imageSize.height = height;
    this->chinesePaintFilterParams.imageSize.channels = 4;
    this->chinesePaintFilterParams.imageSize.bytesPerLine = this->chinesePaintFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(ChinesePaintFilterParams);
    params.paramsData = &this->chinesePaintFilterParams;
    params.shaderPath = SHADER(chinese_paint.comp.glsl.spv);
    return DoApply(gpuCtx, "ChinesePaint", bufferSize, width, height, inputBuffer, outputBuffer, params);
}

void ChinesePaintFilter::Destroy() {
    BasicFilter::Destroy();
}
