//
// Created by neo on 2025/3/14.
//

#include "MultiplyBlender.h"

#include "core/config.h"

VkResult MultiplyBlender::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                const BlendImageInfo baseImageInfo,
                                const BlendImageInfo blendImageInfo,
                                const VkBuffer outputBuffer) {
    BasicBlenderParams params;
    this->multiplyBlenderParams.imageSize.baseImageWidth = baseImageInfo.width;
    this->multiplyBlenderParams.imageSize.baseImageHeight = baseImageInfo.height;
    this->multiplyBlenderParams.imageSize.baseImageChannels = baseImageInfo.channels;
    this->multiplyBlenderParams.imageSize.baseImageBytesPerLine = baseImageInfo.width * baseImageInfo.channels;
    this->multiplyBlenderParams.imageSize.blendImageWidth = blendImageInfo.width;
    this->multiplyBlenderParams.imageSize.blendImageHeight = blendImageInfo.height;
    this->multiplyBlenderParams.imageSize.blendImageChannels = blendImageInfo.channels;
    this->multiplyBlenderParams.imageSize.blendImageBytesPerLine = blendImageInfo.width * blendImageInfo.channels;
    this->multiplyBlenderParams.imageSize.blendImagePosX = blendImageInfo.posX;
    this->multiplyBlenderParams.imageSize.blendImagePosY = blendImageInfo.posY;
    params.paramsSize = sizeof(MultiplyBlenderParams);
    params.paramsData = &this->multiplyBlenderParams;
    params.shaderPath = SHADER(multiply.comp.glsl.spv);
    return DoApply(gpuCtx,
                   "MultiplyBlender",
                   baseImageInfo,
                   blendImageInfo,
                   outputBuffer,
                   params);
}

void MultiplyBlender::Destroy() {
    BaseBlender::Destroy();
}
