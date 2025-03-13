//
// Created by neo on 2025/3/12.
//

#include "ColorSeparationFilter.h"

#include "ColorPencilSketchFilter.h"

VkResult ColorSeparationFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                      const VkDeviceSize bufferSize,
                                      const uint32_t width,
                                      const uint32_t height,
                                      const VkBuffer inputBuffer,
                                      const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->colorSeparationFilterParams.imageSize.width = width;
    this->colorSeparationFilterParams.imageSize.height = height;
    this->colorSeparationFilterParams.imageSize.channels = 4;
    this->colorSeparationFilterParams.imageSize.bytesPerLine =
            this->colorSeparationFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(ColorSeparationFilterParams);
    params.paramsData = &this->colorSeparationFilterParams;
    params.shaderPath = "../../shader_compiled/color_separation.comp.glsl.spv";
    return DoApply(gpuCtx, "ColorSeparation", bufferSize, width, height, inputBuffer, outputBuffer, params);
}

void ColorSeparationFilter::Destroy() {
    BasicFilter::Destroy();
}
