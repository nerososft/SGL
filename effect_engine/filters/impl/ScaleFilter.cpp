//
// Created by neo on 2025/3/13.
//

#include "ScaleFilter.h"

#include "gpu_engine/config.h"

VkResult ScaleFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                            const std::vector<FilterImageInfo> &inputImageInfo,
                            const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->scaleFilterParams.imageSize.width = inputImageInfo[0].width;
    this->scaleFilterParams.imageSize.height = inputImageInfo[0].height;
    this->scaleFilterParams.imageSize.channels = 4;
    this->scaleFilterParams.imageSize.bytesPerLine =
            this->scaleFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(ScaleFilterParams);
    params.paramsData = &this->scaleFilterParams;
    params.shaderPath = SHADER(scale.comp.glsl.spv);
    return BasicFilter::Apply(gpuCtx,
                              "Scale",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (this->scaleFilterParams.targetWidth + 31) / 32,
                              (this->scaleFilterParams.targetHeight + 31) / 32,
                              1);
}

void ScaleFilter::Destroy() {
    BasicFilter::Destroy();
}
