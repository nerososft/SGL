#include "PolarCoordinatesFilter.h"

#include "gpu_engine/config.h"

VkResult PolarCoordinatesFilter::Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
    const std::vector<FilterImageInfo>& inputImageInfo,
    const std::vector<FilterImageInfo>& outputImageInfo) {
    BasicFilterParams params;
    this->polarcoordinatesFilterParams.imageSize.width = inputImageInfo[0].width;;
    this->polarcoordinatesFilterParams.imageSize.height = inputImageInfo[0].height;;
    this->polarcoordinatesFilterParams.imageSize.channels = 4;
    this->polarcoordinatesFilterParams.imageSize.bytesPerLine = this->polarcoordinatesFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(PolarCoordinatesFilterParams);
    params.paramsData = &this->polarcoordinatesFilterParams;
    params.shaderPath = SHADER(polarcoordinates.comp.glsl.spv);

    return BasicFilter::Apply(gpuCtx,
        "polarcoordinates",
        inputImageInfo[0].bufferSize,
        inputImageInfo[0].storageBuffer,
        outputImageInfo[0].storageBuffer,
        params,
        (outputImageInfo[0].width + 31) / 32,
        (outputImageInfo[0].height + 31) / 32,
        1);
}

void PolarCoordinatesFilter::Destroy() {
    BasicFilter::Destroy();
}
