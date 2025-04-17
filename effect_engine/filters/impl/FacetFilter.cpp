#include "FacetFilter.h"

#include "effect_engine/config.h"

VkResult FacetFilter::Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
    const std::vector<FilterImageInfo>& inputImageInfo,
    const std::vector<FilterImageInfo>& outputImageInfo) {
    BasicFilterParams params;
    this->facetParams.imageSize.width = inputImageInfo[0].width;
    this->facetParams.imageSize.height = inputImageInfo[0].height;
    this->facetParams.imageSize.channels = 4;
    this->facetParams.imageSize.bytesPerLine = this->facetParams.imageSize.width * 4;
    params.paramsSize = sizeof(FacetFilterParams);
    params.paramsData = &this->facetParams;
    params.shaderPath = SHADER(facet.comp.glsl.spv);

    return BasicFilter::Apply(gpuCtx,
        "Facet",
        inputImageInfo[0].bufferSize,
        inputImageInfo[0].storageBuffer,
        outputImageInfo[0].storageBuffer,
        params,
        (outputImageInfo[0].width + 31) / 32,
        (outputImageInfo[0].height + 31) / 32,
        1);
}

void FacetFilter::Destroy() {
    BasicFilter::Destroy();
}
