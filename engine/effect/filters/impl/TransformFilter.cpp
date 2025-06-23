//
// Created by neo on 25-6-18.
//

#include "TransformFilter.h"

#include "core/config.h"

VkResult TransformFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                const std::vector<FilterImageInfo> &inputImageInfo,
                                const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->transformFilterParams.imageSize.width = inputImageInfo[0].width;;
    this->transformFilterParams.imageSize.height = inputImageInfo[0].height;;
    this->transformFilterParams.imageSize.channels = 4;
    this->transformFilterParams.imageSize.bytesPerLine = this->transformFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(TransformFilterParams);
    params.paramsData = &this->transformFilterParams;
    params.shaderPath = SHADER(transform.comp.glsl.spv);

    return BasicFilter::Apply(gpuCtx,
                              "Transform",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (outputImageInfo[0].width + 31) / 32,
                              (outputImageInfo[0].height + 31) / 32,
                              1);
}

void TransformFilter::Destroy() {
    BasicFilter::Destroy();
}
