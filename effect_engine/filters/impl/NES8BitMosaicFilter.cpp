//
// Created by neo on 2025/3/19.
//

#include "NES8BitMosaicFilter.h"

#include "effect_engine/config.h"

VkResult NES8BitMosaicFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                    const std::vector<FilterImageInfo> &inputImageInfo,
                                    const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->nes8BitMosaicFilterParams.imageSize.width = inputImageInfo[0].width;
    this->nes8BitMosaicFilterParams.imageSize.height = inputImageInfo[0].height;
    this->nes8BitMosaicFilterParams.imageSize.channels = 4;
    this->nes8BitMosaicFilterParams.imageSize.bytesPerLine =
            this->nes8BitMosaicFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(NES8BitMosaicFilterParams);
    params.paramsData = &this->nes8BitMosaicFilterParams;
    params.shaderPath = SHADER(nes8bit.comp.glsl.spv);
    return BasicFilter::Apply(gpuCtx,
                              "NES8BitMosaicFilter",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (outputImageInfo[0].width + 31) / 32,
                              (outputImageInfo[0].height + 31) / 32,
                              1);
}

void NES8BitMosaicFilter::Destroy() {
    BasicFilter::Destroy();
}
