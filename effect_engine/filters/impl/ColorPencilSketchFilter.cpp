//
// Created by neo on 2025/3/12.
//

#include "ColorPencilSketchFilter.h"

#include "gpu_engine/config.h"

VkResult ColorPencilSketchFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                        const std::vector<FilterImageInfo> &inputImageInfo,
                                        const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->colorPencilSketchFilterParams.imageSize.width = inputImageInfo[0].width;
    this->colorPencilSketchFilterParams.imageSize.height = inputImageInfo[0].height;
    this->colorPencilSketchFilterParams.imageSize.channels = 4;
    this->colorPencilSketchFilterParams.imageSize.bytesPerLine =
            this->colorPencilSketchFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(ColorPencilSketchFilterParams);
    params.paramsData = &this->colorPencilSketchFilterParams;
    params.shaderPath = SHADER(color_pencil_sketch.comp.glsl.spv);
    return BasicFilter::Apply(gpuCtx,
                              "ColorPencilSketch",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (outputImageInfo[0].width + 15) / 16,
                              (outputImageInfo[0].height + 15) / 16,
                              1);
}

void ColorPencilSketchFilter::Destroy() {
    BasicFilter::Destroy();
}
