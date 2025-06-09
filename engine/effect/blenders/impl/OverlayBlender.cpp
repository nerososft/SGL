//
// Created by neo on 2025/3/14.
//

#include "OverlayBlender.h"

#include "core/config.h"

VkResult OverlayBlender::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                               const BlendImageInfo baseImageInfo,
                               const BlendImageInfo blendImageInfo,
                               const VkBuffer outputBuffer) {
    BasicBlenderParams params;
    this->overlayBlenderParams.imageSize.baseImageWidth = baseImageInfo.width;
    this->overlayBlenderParams.imageSize.baseImageHeight = baseImageInfo.height;
    this->overlayBlenderParams.imageSize.baseImageChannels = baseImageInfo.channels;
    this->overlayBlenderParams.imageSize.baseImageBytesPerLine = baseImageInfo.width * baseImageInfo.channels;
    this->overlayBlenderParams.imageSize.blendImageWidth = blendImageInfo.width;
    this->overlayBlenderParams.imageSize.blendImageHeight = blendImageInfo.height;
    this->overlayBlenderParams.imageSize.blendImageChannels = blendImageInfo.channels;
    this->overlayBlenderParams.imageSize.blendImageBytesPerLine = blendImageInfo.width * blendImageInfo.channels;
    this->overlayBlenderParams.imageSize.blendImagePosX = blendImageInfo.posX;
    this->overlayBlenderParams.imageSize.blendImagePosY = blendImageInfo.posY;
    params.paramsSize = sizeof(OverlayBlenderParams);
    params.paramsData = &this->overlayBlenderParams;
    params.shaderPath = SHADER(overlay.comp.glsl.spv);
    return DoApply(gpuCtx,
                   "OverlayBlender",
                   baseImageInfo,
                   blendImageInfo,
                   outputBuffer,
                   params);
}

void OverlayBlender::Destroy() {
    BaseBlender::Destroy();
}
