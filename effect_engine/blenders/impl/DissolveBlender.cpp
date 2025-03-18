//
// Created by neo on 2025/3/14.
//

#include "DissolveBlender.h"

#include "effect_engine/config.h"

VkResult DissolveBlender::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                const BlendImageInfo baseImageInfo,
                                const BlendImageInfo blendImageInfo,
                                const VkBuffer outputBuffer) {
    BasicBlenderParams params;
    this->dissolveBlenderParams.imageSize.baseImageWidth = baseImageInfo.width;
    this->dissolveBlenderParams.imageSize.baseImageHeight = baseImageInfo.height;
    this->dissolveBlenderParams.imageSize.baseImageChannels = baseImageInfo.channels;
    this->dissolveBlenderParams.imageSize.baseImageBytesPerLine = baseImageInfo.width * baseImageInfo.channels;
    this->dissolveBlenderParams.imageSize.blendImageWidth = blendImageInfo.width;
    this->dissolveBlenderParams.imageSize.blendImageHeight = blendImageInfo.height;
    this->dissolveBlenderParams.imageSize.blendImageChannels = blendImageInfo.channels;
    this->dissolveBlenderParams.imageSize.blendImageBytesPerLine = blendImageInfo.width * blendImageInfo.channels;
    this->dissolveBlenderParams.imageSize.blendImagePosX = blendImageInfo.posX;
    this->dissolveBlenderParams.imageSize.blendImagePosY = blendImageInfo.posY;
    params.paramsSize = sizeof(DissolveBlenderParams);
    params.paramsData = &this->dissolveBlenderParams;
    params.shaderPath = SHADER(dissolve.comp.glsl.spv);
    return DoApply(gpuCtx,
                   "DissolveBlender",
                   baseImageInfo,
                   blendImageInfo,
                   outputBuffer,
                   params);
}

void DissolveBlender::Destroy() {
    BaseBlender::Destroy();
}
