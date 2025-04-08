//
// Created by neo on 2025/3/11.
//

#include "DistortGlassFilter.h"

#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif
#include "effect_engine/config.h"
#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUHelper.h"
#include "effect_engine/gpu/compute_graph/BufferCopyNode.h"
#include "effect_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "effect_engine/log/Log.h"

VkResult DistortGlassFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                   const VkDeviceSize bufferSize,
                                   const uint32_t width,
                                   const uint32_t height,
                                   const VkBuffer inputBuffer,
                                   const VkBuffer outputBuffer) {



    BasicFilterParams params;
    this->glassFilterParams.imageSize.width = width;
    this->glassFilterParams.imageSize.height = height;
    this->glassFilterParams.imageSize.channels = 4;
    this->glassFilterParams.imageSize.bytesPerLine = this->glassFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(DistortGlassFilterParams);
    params.paramsData = &this->glassFilterParams;
    params.shaderPath = SHADER(distort_glass.comp.glsl.spv);




    return DoApply(gpuCtx,
        "DistortGlass",
        bufferSize,
        inputBuffer,
        outputBuffer,
        params,
        (width + 31) / 32,
        (height + 31) / 32,
        1);

}


void DistortGlassFilter::Destroy() {
    BasicFilter::Destroy();
}
