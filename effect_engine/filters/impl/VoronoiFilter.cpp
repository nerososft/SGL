//
// Created by neo on 2025/3/19.
//

#include "VoronoiFilter.h"

#include "effect_engine/config.h"

VkResult VoronoiFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                              VkDeviceSize bufferSize,
                              uint32_t width,
                              uint32_t height,
                              VkBuffer inputBuffer,
                              VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->voronoiFilterParams.imageSize.width = width;
    this->voronoiFilterParams.imageSize.height = height;
    this->voronoiFilterParams.imageSize.channels = 4;
    this->voronoiFilterParams.imageSize.bytesPerLine = this->voronoiFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(VoronoiFilterParams);
    params.paramsData = &this->voronoiFilterParams;
    params.shaderPath = SHADER(voronoi.comp.glsl.spv);
    return DoApply(gpuCtx,
                   "VoronoiFilter",
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   params,
                   (width + 31) / 32,
                   (height + 31) / 32,
                   1);
}

void VoronoiFilter::Destroy() {
    BasicFilter::Destroy();
}
