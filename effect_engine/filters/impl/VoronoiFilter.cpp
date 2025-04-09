//
// Created by neo on 2025/3/19.
//

#include "VoronoiFilter.h"

#include "effect_engine/config.h"

VkResult VoronoiFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                              const std::vector<FilterImageInfo> &inputImageInfo,
                              const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->voronoiFilterParams.imageSize.width = inputImageInfo[0].width;
    this->voronoiFilterParams.imageSize.height = inputImageInfo[0].height;
    this->voronoiFilterParams.imageSize.channels = 4;
    this->voronoiFilterParams.imageSize.bytesPerLine =
            this->voronoiFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(VoronoiFilterParams);
    params.paramsData = &this->voronoiFilterParams;
    params.shaderPath = SHADER(voronoi.comp.glsl.spv);
    return BasicFilter::Apply(gpuCtx,
                              "VoronoiFilter",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (outputImageInfo[0].width + 31) / 32,
                              (outputImageInfo[0].height + 31) / 32,
                              1);
}

void VoronoiFilter::Destroy() {
    BasicFilter::Destroy();
}
