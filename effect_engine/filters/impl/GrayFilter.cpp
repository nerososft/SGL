//
// Created by 1234 on 2025/3/6.
//

#include "GrayFilter.h"

VkResult GrayFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           const VkDeviceSize bufferSize,
                           const uint32_t width,
                           const uint32_t height,
                           const VkBuffer inputBuffer,
                           const VkBuffer outputBuffer) {
    FilterParams params;
    params.paramsSize = sizeof(GrayFilterParams);
    params.paramsData = &this->grayFilterParams;
    params.shaderPath = "../shader_compiled/gray.comp.glsl.spv";
    return DoApply(gpuCtx, bufferSize, width, height, inputBuffer, outputBuffer, params);
}
