//
// Created by neo on 2025/3/13.
//

#include "FocusBlurFilter.h"

#include "core/config.h"

VkResult FocusBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                const std::vector<FilterImageInfo> &inputImageInfo,
                                const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->focusBlurFilterParams.imageSize.width = inputImageInfo[0].width;
    this->focusBlurFilterParams.imageSize.height = inputImageInfo[0].height;
    this->focusBlurFilterParams.imageSize.channels = 4;
    this->focusBlurFilterParams.imageSize.bytesPerLine =
            this->focusBlurFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(FocusBlurFilterParams);
    params.paramsData = &this->focusBlurFilterParams;
    params.shaderPath = SHADER(focus_blur.comp.glsl.spv);
    return BasicFilter::Apply(gpuCtx,
                              "FocusBlur",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (outputImageInfo[0].width + 15) / 16,
                              (outputImageInfo[0].height + 15) / 16,
                              1);
}

void FocusBlurFilter::Destroy() {
    BasicFilter::Destroy();
}
