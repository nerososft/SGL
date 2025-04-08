//
// Created by neo on 2025/3/11.
//

#include "BlurEdgeFilter.h"

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

VkResult BlurEdgeFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                   const VkDeviceSize bufferSize,
                                   const uint32_t width,
                                   const uint32_t height,
                                   const VkBuffer inputBuffer,
                                   const VkBuffer outputBuffer) {



    BasicFilterParams params;
    this->blurEdgeFilterParams.imageSize.width = width;
    this->blurEdgeFilterParams.imageSize.height = height;
    this->blurEdgeFilterParams.imageSize.channels = 4;
    this->blurEdgeFilterParams.imageSize.bytesPerLine = this->blurEdgeFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(BlurEdgeFilterParams);
    params.paramsData = &this->blurEdgeFilterParams;
    params.shaderPath = SHADER(blur_edge.comp.glsl.spv);

    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);


    return DoApply(gpuCtx,
        "BlurEdge",
        bufferSize,
        inputBuffer,
        outputBuffer,
        params,
        (width + 31) / 32,
        (height + 31) / 32,
        1);

}


void BlurEdgeFilter::Destroy() {
    this->computeGraph->Destroy();
    computeGraph->Destroy();
    //BasicFilter::Destroy();
}
