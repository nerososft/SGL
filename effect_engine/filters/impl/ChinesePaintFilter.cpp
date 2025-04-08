//
// Created by 1234 on 2025/3/6.
//

#include "ChinesePaintFilter.h"

#include "effect_engine/config.h"

VkResult ChinesePaintFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                   const std::vector<FilterImageInfo> inputImageInfo,
                                   const std::vector<FilterImageInfo> outputImageInfo) {
    BasicFilterParams params;
    this->chinesePaintFilterParams.imageSize.width = inputImageInfo[0].width;
    this->chinesePaintFilterParams.imageSize.height = inputImageInfo[0].height;
    this->chinesePaintFilterParams.imageSize.channels = 4;
    this->chinesePaintFilterParams.imageSize.bytesPerLine = this->chinesePaintFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(ChinesePaintFilterParams);
    params.paramsData = &this->chinesePaintFilterParams;
    params.shaderPath = SHADER(chinese_paint.comp.glsl.spv);
    return BasicFilter::Apply(gpuCtx,
                              "ChinesePaint",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (outputImageInfo[0].width + 15) / 16,
                              (outputImageInfo[0].height + 15) / 16,
                              1);
}

void ChinesePaintFilter::Destroy() {
    BasicFilter::Destroy();
}
