//
// Created by neo on 2025/3/18.
//

#include "PaletteKnifeFilter.h"

#include "effect_engine/config.h"

VkResult PaletteKnifeFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                   const VkDeviceSize bufferSize,
                                   const uint32_t width,
                                   const uint32_t height,
                                   const VkBuffer inputBuffer,
                                   const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->paletteKnifeFilterParams.imageSize.width = width;
    this->paletteKnifeFilterParams.imageSize.height = height;
    this->paletteKnifeFilterParams.imageSize.channels = 4;
    this->paletteKnifeFilterParams.imageSize.bytesPerLine = this->paletteKnifeFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(PaletteKnifeFilterParams);
    params.paramsData = &this->paletteKnifeFilterParams;
    params.shaderPath = SHADER(palette_knife.comp.glsl.spv);
    return DoApply(gpuCtx,
                   "PaletteKnifeFilter",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (width + 31) / 32,
                   (height + 31) / 32,
                   1);
}

void PaletteKnifeFilter::Destroy() {
    BasicFilter::Destroy();
}
