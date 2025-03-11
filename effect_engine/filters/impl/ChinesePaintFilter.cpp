//
// Created by 1234 on 2025/3/6.
//

#include "ChinesePaintFilter.h"

VkResult ChinesePaintFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                   const VkDeviceSize bufferSize,
                                   const uint32_t width,
                                   const uint32_t height,
                                   const VkBuffer inputBuffer,
                                   const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->chinsesPaintFilterParams.imageSize.width = width;
    this->chinsesPaintFilterParams.imageSize.height = height;
    this->chinsesPaintFilterParams.imageSize.channels = 4;
    this->chinsesPaintFilterParams.imageSize.bytesPerLine = this->chinsesPaintFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(ChinesePaintFilterParams);
    params.paramsData = &this->chinsesPaintFilterParams;
    params.shaderPath = "../../shader_compiled/chinese_paint.comp.glsl.spv";
    return DoApply(gpuCtx, "ChinesePaint", bufferSize, width, height, inputBuffer, outputBuffer, params);
}
