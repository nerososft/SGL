//
// Created by neo on 2025/3/13.
//

#include "FocusBlurFilter.h"

VkResult FocusBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                const VkDeviceSize bufferSize,
                                const uint32_t width,
                                const uint32_t height,
                                const VkBuffer inputBuffer,
                                const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->focusBlurFilterParams.imageSize.width = width;
    this->focusBlurFilterParams.imageSize.height = height;
    this->focusBlurFilterParams.imageSize.channels = 4;
    this->focusBlurFilterParams.imageSize.bytesPerLine = this->focusBlurFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(FocusBlurFilterParams);
    params.paramsData = &this->focusBlurFilterParams;
    params.shaderPath = "../../shader_compiled/focus_blur.comp.glsl.spv";
    return DoApply(gpuCtx,
                   "FocusBlur",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (width + 15) / 16,
                   (height + 15) / 16,
                   1);
}

void FocusBlurFilter::Destroy() {
    BasicFilter::Destroy();
}
