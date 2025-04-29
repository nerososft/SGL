#include "WaveFilter.h"

#include "effect_engine/config.h"

VkResult WaveFilter::Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
    const std::vector<FilterImageInfo>& inputImageInfo,
    const std::vector<FilterImageInfo>& outputImageInfo) {
    BasicFilterParams params;
    this->waveFilterParams.imageSize.width = inputImageInfo[0].width;;
    this->waveFilterParams.imageSize.height = inputImageInfo[0].height;;
    this->waveFilterParams.imageSize.channels = 4;
    this->waveFilterParams.imageSize.bytesPerLine = this->waveFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(WaveFilterParams);
    params.paramsData = &this->waveFilterParams;
    params.shaderPath = SHADER(wave.comp.glsl.spv);

    return BasicFilter::Apply(gpuCtx,
        "Wave",
        inputImageInfo[0].bufferSize,
        inputImageInfo[0].storageBuffer,
        outputImageInfo[0].storageBuffer,
        params,
        (outputImageInfo[0].width + 31) / 32,
        (outputImageInfo[0].height + 31) / 32,
        1);
}

void WaveFilter::Destroy() {
    BasicFilter::Destroy();
}