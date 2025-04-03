//
// Created by neo on 2025/3/13.
//

#include "ScaleFilter.h"

#include "effect_engine/config.h"

VkResult ScaleFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                            const VkDeviceSize bufferSize,
                            const uint32_t width,
                            const uint32_t height,
                            const VkBuffer inputBuffer,
                            const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->scaleFilterParams.imageSize.width = width;
    this->scaleFilterParams.imageSize.height = height;
    this->scaleFilterParams.imageSize.channels = 4;
    this->scaleFilterParams.imageSize.bytesPerLine = this->scaleFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(ScaleFilterParams);
    params.paramsData = &this->scaleFilterParams;
    params.shaderPath = SHADER(scale.comp.glsl.spv);

    return DoApply(gpuCtx,
                   "Scale",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (this->scaleFilterParams.targetWidth + 31) / 32,
                   (this->scaleFilterParams.targetHeight + 31) / 32,
                   1);
}

void ScaleFilter::Destroy() {
    BasicFilter::Destroy();
}
