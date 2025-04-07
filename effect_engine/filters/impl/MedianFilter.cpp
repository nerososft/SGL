//
// Created by neo on 25-4-3.
//

#include "MedianFilter.h"

#include <memory>
#include <effect_engine/config.h>
#include <effect_engine/filters/BasicFilter.h>
#include <effect_engine/gpu/VkGPUContext.h>
#include <vulkan/vulkan_core.h>

VkResult MedianFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                             const VkDeviceSize bufferSize,
                             const uint32_t width,
                             const uint32_t height,
                             const VkBuffer inputBuffer,
                             const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->medianFilterParams.imageSize.width = width;
    this->medianFilterParams.imageSize.height = height;
    this->medianFilterParams.imageSize.channels = 4;
    this->medianFilterParams.imageSize.bytesPerLine = this->medianFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(MedianFilterParams);
    params.paramsData = &this->medianFilterParams;
    params.shaderPath = SHADER(midvalue.comp.glsl.spv);

    return DoApply(gpuCtx,
                   "MedianFilter",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (this->medianFilterParams.imageSize.width + 31) / 32,
                   (this->medianFilterParams.imageSize.height + 31) / 32,
                   1);
}

void MedianFilter::Destroy() {
    BasicFilter::Destroy();
}
