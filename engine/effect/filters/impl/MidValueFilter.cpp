#include "MidValueFilter.h"

#include "core/config.h"

VkResult MidValueFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                               const std::vector<FilterImageInfo> &inputImageInfo,
                               const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->midvalueFilterParams.imageSize.width = inputImageInfo[0].width;;
    this->midvalueFilterParams.imageSize.height = inputImageInfo[0].height;;
    this->midvalueFilterParams.imageSize.channels = 4;
    this->midvalueFilterParams.imageSize.bytesPerLine = this->midvalueFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(MidValueFilterParams);
    params.paramsData = &this->midvalueFilterParams;
    params.shaderPath = SHADER(midvalue.comp.glsl.spv);

    return BasicFilter::Apply(gpuCtx,
                              "MidValue",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (outputImageInfo[0].width + 31) / 32,
                              (outputImageInfo[0].height + 31) / 32,
                              1);
}

void MidValueFilter::Destroy() {
    BasicFilter::Destroy();
}
