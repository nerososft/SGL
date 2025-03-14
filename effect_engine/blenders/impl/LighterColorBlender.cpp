//
// Created by Jun Li on 2025/3/14.
//

#include "LighterColorBlender.h"

VkResult LighterColorBlender::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                const BlendImageInfo baseImageInfo,
                                const BlendImageInfo blendImageInfo,
                                const VkBuffer outputBuffer) {
    BasicBlenderParams params;
    this->lighterColorBlenderParams.imageSize.baseImageWidth = baseImageInfo.width;
    this->lighterColorBlenderParams.imageSize.baseImageHeight = baseImageInfo.height;
    this->lighterColorBlenderParams.imageSize.baseImageChannels = baseImageInfo.channels;
    this->lighterColorBlenderParams.imageSize.baseImageBytesPerLine = baseImageInfo.width * baseImageInfo.channels;
    this->lighterColorBlenderParams.imageSize.blendImageWidth = blendImageInfo.width;
    this->lighterColorBlenderParams.imageSize.blendImageHeight = blendImageInfo.height;
    this->lighterColorBlenderParams.imageSize.blendImageChannels = blendImageInfo.channels;
    this->lighterColorBlenderParams.imageSize.blendImageBytesPerLine = blendImageInfo.width * blendImageInfo.channels;
    this->lighterColorBlenderParams.imageSize.blendImagePosX = blendImageInfo.posX;
    this->lighterColorBlenderParams.imageSize.blendImagePosY = blendImageInfo.posY;
    this->lighterColorBlenderParams.brightnessThreshold = 0.5f; // 默认阈值
    this->lighterColorBlenderParams.blendFactor = 0.5f;      // 默认混合因子

    params.paramsSize = sizeof(LighterColorBlenderParams);
    params.paramsData = &this->lighterColorBlenderParams;
    params.shaderPath = "../../shader_compiled/lighter_color.comp.glsl.spv";

    return DoApply(gpuCtx,
                   "LightBlender",
                   baseImageInfo,
                   blendImageInfo,
                   outputBuffer,
                   params);
}

void LighterColorBlender::Destroy() {
    BaseBlender::Destroy();
}