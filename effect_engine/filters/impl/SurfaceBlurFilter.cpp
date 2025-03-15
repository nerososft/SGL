//
// Created by Jun Li on 2025/3/13.
//

// SurfaceBlurFilter.cpp
#include "SurfaceBlurFilter.h"

VkResult SurfaceBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                  const VkDeviceSize bufferSize,
                                  const uint32_t width,
                                  const uint32_t height,
                                  const VkBuffer inputBuffer,
                                  const VkBuffer outputBuffer) {
    BasicFilterParams params;
    blurParams.imageSize.width = width;
    blurParams.imageSize.height = height;
    blurParams.imageSize.channels = 4;
    blurParams.imageSize.bytesPerLine = width * 4;

    params.paramsSize = sizeof(SurfaceBlurFilterParams);
    params.paramsData = &blurParams;
    params.shaderPath = "../../shader_compiled/surface_blur.comp.glsl.spv";

    return DoApply(gpuCtx,
                   "SurfaceBlur",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (width + 15) / 16,
                   (height + 15) / 16,
                   1);
}

void SurfaceBlurFilter::Destroy() {
    BasicFilter::Destroy();
}
