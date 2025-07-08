//
// Created by neo on 25-7-7.
//

#include "YuvToRgbFilter.h"

#include "core/config.h"

VkResult YuvToRgbFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                               const std::vector<FilterImageInfo> &inputImageInfo,
                               const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->yuvToRgbFilterParams.imageSize.width = inputImageInfo[0].width;
    this->yuvToRgbFilterParams.imageSize.height = inputImageInfo[0].height;
    this->yuvToRgbFilterParams.imageSize.channels = 4;
    this->yuvToRgbFilterParams.imageSize.bytesPerLine =
            this->yuvToRgbFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(YuvToRgbFilterParams);
    params.paramsData = &this->yuvToRgbFilterParams;
    params.shaderPath = SHADER(yuv2rgb.comp.glsl.spv);
    return BasicFilter::Apply(gpuCtx,
                              "YuvToRgb",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (outputImageInfo[0].width + 31) / 32,
                              (outputImageInfo[0].height + 31) / 32,
                              1);
}

void YuvToRgbFilter::Destroy() {
    BasicFilter::Destroy();
}
