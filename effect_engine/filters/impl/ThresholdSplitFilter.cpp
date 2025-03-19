//
// Created by 1234 on 2025/3/6.
//

#include "ThresholdSplitFilter.h"

#include "effect_engine/config.h"

VkResult ThresholdSplitFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                     const VkDeviceSize bufferSize,
                                     const uint32_t width,
                                     const uint32_t height,
                                     const VkBuffer inputBuffer,
                                     const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->thresholdSplitFilterParams.imageSize.width = width;
    this->thresholdSplitFilterParams.imageSize.height = height;
    this->thresholdSplitFilterParams.imageSize.channels = 4;
    this->thresholdSplitFilterParams.imageSize.bytesPerLine = this->thresholdSplitFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(ThresholdSplitFilterParams);
    params.paramsData = &this->thresholdSplitFilterParams;
    params.shaderPath = SHADER(threshold_split.comp.glsl.spv);
    return DoApply(gpuCtx,
                   "ThresholdSplit",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (width + 31) / 32,
                   (height + 31) / 32,
                   1);
}

void ThresholdSplitFilter::Destroy() {
    BasicFilter::Destroy();
}
