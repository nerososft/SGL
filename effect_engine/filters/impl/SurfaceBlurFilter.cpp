//
// Created by Jun Li on 2025/3/13.
//

// SurfaceBlurFilter.cpp
#include "SurfaceBlurFilter.h"

#include "effect_engine/config.h"

VkResult SurfaceBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                  const std::vector<FilterImageInfo> inputImageInfo,
                                  const std::vector<FilterImageInfo> outputImageInfo) {
    BasicFilterParams params;
    this->blurParams.imageSize.width = inputImageInfo[0].width;
    this->blurParams.imageSize.height = inputImageInfo[0].height;
    this->blurParams.imageSize.channels = 4;
    this->blurParams.imageSize.bytesPerLine =
            this->blurParams.imageSize.width * 4;
    params.paramsSize = sizeof(SurfaceBlurFilterParams);
    params.paramsData = &this->blurParams;
    params.shaderPath = SHADER(surface_blur.comp.glsl.spv);
    return BasicFilter::Apply(gpuCtx,
                              "SurfaceBlur",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (outputImageInfo[0].width + 15) / 16,
                              (outputImageInfo[0].height + 15) / 16,
                              1);
}

void SurfaceBlurFilter::Destroy() {
    BasicFilter::Destroy();
}
