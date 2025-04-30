#include "TwirlWarpFilter.h"

#include "gpu_engine/config.h"

VkResult TwirlWarpFilter::Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
    const std::vector<FilterImageInfo>& inputImageInfo,
    const std::vector<FilterImageInfo>& outputImageInfo) {
    BasicFilterParams params;
    this->twirlwarpFilterParams.imageSize.width = inputImageInfo[0].width;;
    this->twirlwarpFilterParams.imageSize.height = inputImageInfo[0].height;;
    this->twirlwarpFilterParams.imageSize.channels = 4;
    this->twirlwarpFilterParams.imageSize.bytesPerLine = this->twirlwarpFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(TwirlWarpFilterParams);
    params.paramsData = &this->twirlwarpFilterParams;
    params.shaderPath = SHADER(twirlwarp.comp.glsl.spv);

    return BasicFilter::Apply(gpuCtx,
        "TwirlWarp",
        inputImageInfo[0].bufferSize,
        inputImageInfo[0].storageBuffer,
        outputImageInfo[0].storageBuffer,
        params,
        (outputImageInfo[0].width + 31) / 32,
        (outputImageInfo[0].height + 31) / 32,
        1);
}

void TwirlWarpFilter::Destroy() {
    BasicFilter::Destroy();
}