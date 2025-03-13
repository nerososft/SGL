//
// Created by Jun Li on 2025/3/13.
//

#include "AverageBlurFilter.h"

VkResult AverageBlurFilter::Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
                                  VkDeviceSize bufferSize,
                                  uint32_t width,
                                  uint32_t height,
                                  VkBuffer inputBuffer,
                                  VkBuffer outputBuffer)
{
    BasicFilterParams params;
    blurParams.imageSize.width = width;
    blurParams.imageSize.height = height;
    blurParams.imageSize.channels = 4;
    blurParams.imageSize.bytesPerLine = width * 4;

    params.paramsSize = sizeof(AverageBlurFilterParams);
    params.paramsData = &blurParams;
    params.shaderPath = "../../shader_compiled/average_blur.comp.glsl.spv";

    return DoApply(gpuCtx,
                   "AverageBlur",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (width + 15) / 16,
                   (height + 15) / 16,
                   1);
}

void AverageBlurFilter::Destroy() {
    BasicFilter::Destroy();
}
