#include "SpherizeFilter.h"

#include "gpu_engine/config.h"

VkResult SpherizeFilter::Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
    const std::vector<FilterImageInfo>& inputImageInfo,
    const std::vector<FilterImageInfo>& outputImageInfo) {
    BasicFilterParams params;
    this->spherizeFilterParams.imageSize.width = inputImageInfo[0].width;;
    this->spherizeFilterParams.imageSize.height = inputImageInfo[0].height;;
    this->spherizeFilterParams.imageSize.channels = 4;
    this->spherizeFilterParams.imageSize.bytesPerLine = this->spherizeFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(SpherizeFilterParams);
    params.paramsData = &this->spherizeFilterParams;
    params.shaderPath = SHADER(spherize.comp.glsl.spv);

    return BasicFilter::Apply(gpuCtx,
        "Spherize",
        inputImageInfo[0].bufferSize,
        inputImageInfo[0].storageBuffer,
        outputImageInfo[0].storageBuffer,
        params,
        (outputImageInfo[0].width + 31) / 32,
        (outputImageInfo[0].height + 31) / 32,
        1);
}

void SpherizeFilter::Destroy() {
    BasicFilter::Destroy();
}