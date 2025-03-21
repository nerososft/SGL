//
// Created by neo on 2025/3/19.
//

#include "NES8BitMosaicFilter.h"

#include "effect_engine/config.h"

VkResult NES8BitMosaicFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                    const VkDeviceSize bufferSize,
                                    const uint32_t width,
                                    const uint32_t height,
                                    const VkBuffer inputBuffer,
                                    const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->nes8BitMosaicFilterParams.imageSize.width = width;
    this->nes8BitMosaicFilterParams.imageSize.height = height;
    this->nes8BitMosaicFilterParams.imageSize.channels = 4;
    this->nes8BitMosaicFilterParams.imageSize.bytesPerLine = this->nes8BitMosaicFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(NES8BitMosaicFilterParams);
    params.paramsData = &this->nes8BitMosaicFilterParams;
    params.shaderPath = SHADER(nes8bit.comp.glsl.spv);
    return DoApply(gpuCtx,
                   "NES8BitMosaicFilter",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (width + 31) / 32,
                   (height + 31) / 32,
                   1);
}

void NES8BitMosaicFilter::Destroy() {
    BasicFilter::Destroy();
}
