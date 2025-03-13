//
// Created by neo on 2025/3/13.
//

#include "ScaleFilter.h"

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
    params.shaderPath = "../../shader_compiled/scale.comp.glsl.spv";
    return DoApply(gpuCtx,
                   "Scale",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (width + 31) / 32,
                   (height + 31) / 32,
                   1);
}

void ScaleFilter::Destroy() {
    BasicFilter::Destroy();
}
