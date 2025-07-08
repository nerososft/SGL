//
// Created by neo on 25-7-8.
//

#include "RgbToYuvFilter.h"

#include <core/config.h>

VkResult RgbToYuvFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                               const std::vector<FilterImageInfo> &inputImageInfo,
                               const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->rgbToYuvFilterParams.imageSize.width = inputImageInfo[0].width;
    this->rgbToYuvFilterParams.imageSize.height = inputImageInfo[0].height;
    this->rgbToYuvFilterParams.imageSize.channels = 4;
    this->rgbToYuvFilterParams.imageSize.bytesPerLine =
            this->rgbToYuvFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(RgbToYuvFilterParams);
    params.paramsData = &this->rgbToYuvFilterParams;
    params.shaderPath = SHADER(rgb2yuv.comp.glsl.spv);
    return BasicFilter::Apply(gpuCtx,
                              "RgbToYuv",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (outputImageInfo[0].width + 31) / 32,
                              (outputImageInfo[0].height + 31) / 32,
                              1);
}

void RgbToYuvFilter::Destroy() {
    BasicFilter::Destroy();
}
