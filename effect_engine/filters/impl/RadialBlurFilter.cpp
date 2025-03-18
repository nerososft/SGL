//
// Created by neo on 2025/3/13.
//

#include "RadialBlurFilter.h"

#include "effect_engine/config.h"

VkResult RadialBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                 const VkDeviceSize bufferSize,
                                 const uint32_t width,
                                 const uint32_t height,
                                 const VkBuffer inputBuffer,
                                 const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->radialBlurFilterParams.imageSize.width = width;
    this->radialBlurFilterParams.imageSize.height = height;
    this->radialBlurFilterParams.imageSize.channels = 4;
    this->radialBlurFilterParams.imageSize.bytesPerLine = this->radialBlurFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(RadialBlurFilterParams);
    params.paramsData = &this->radialBlurFilterParams;
    params.shaderPath = SHADER(radial_blur.comp.glsl.spv);
    return DoApply(gpuCtx,
                   "RadialBlur",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (width + 15) / 16,
                   (height + 15) / 16,
                   1);
}

void RadialBlurFilter::Destroy() {
    BasicFilter::Destroy();
}
