//
// Created by neo on 2025/3/12.
//

#include "ColorPencilSketchFilter.h"

#include "effect_engine/config.h"

VkResult ColorPencilSketchFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                        const VkDeviceSize bufferSize,
                                        const uint32_t width,
                                        const uint32_t height,
                                        const VkBuffer inputBuffer,
                                        const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->colorPencilSketchFilterParams.imageSize.width = width;
    this->colorPencilSketchFilterParams.imageSize.height = height;
    this->colorPencilSketchFilterParams.imageSize.channels = 4;
    this->colorPencilSketchFilterParams.imageSize.bytesPerLine =
            this->colorPencilSketchFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(ColorPencilSketchFilterParams);
    params.paramsData = &this->colorPencilSketchFilterParams;
    params.shaderPath = SHADER(color_pencil_sketch.comp.glsl.spv);
    return DoApply(gpuCtx, "ColorPencilSketch", bufferSize, width, height, inputBuffer, outputBuffer, params);
}

void ColorPencilSketchFilter::Destroy() {
    BasicFilter::Destroy();
}
