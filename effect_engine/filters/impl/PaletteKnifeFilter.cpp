//
// Created by neo on 2025/3/11.
//

#include "PaletteKnifeFilter.h"

#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif
#include "effect_engine/config.h"
#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUHelper.h"
#include "effect_engine/gpu/compute_graph/BufferCopyComputeGraphNode.h"
#include "effect_engine/gpu/compute_graph/PipelineComputeGraphNode.h"
#include "effect_engine/log/Log.h"

VkResult PaletteKnifeFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                   const VkDeviceSize bufferSize,
                                   const uint32_t width,
                                   const uint32_t height,
                                   const VkBuffer inputBuffer,
                                   const VkBuffer outputBuffer) {
    this->paletteKnifeFilterParams.imageSize.width = width;
    this->paletteKnifeFilterParams.imageSize.height = height;
    this->paletteKnifeFilterParams.imageSize.channels = 4;
    this->paletteKnifeFilterParams.imageSize.bytesPerLine = this->paletteKnifeFilterParams.imageSize.width * 4;

    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    VkResult ret = this->computeGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(PaletteKnifeFilterParams);
    pushConstantInfo.data = &this->paletteKnifeFilterParams;

    PipelineNodeBuffer qPipelineNodeInput;
    qPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    qPipelineNodeInput.buffer = inputBuffer;
    qPipelineNodeInput.bufferSize = bufferSize;

    qBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    qBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_LOCAL, bufferSize);

    PipelineNodeBuffer qPipelineNodeOutput;
    qPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    qPipelineNodeOutput.buffer = qBuffer->GetBuffer();
    qPipelineNodeOutput.bufferSize = bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(qPipelineNodeInput);
    vPipelineBuffers.push_back(qPipelineNodeOutput);

    const auto qCalculateNode = std::make_shared<PipelineComputeGraphNode>(gpuCtx,
                                                                           "QCalculate",
                                                                           SHADER(palette_q.comp.glsl.spv),
                                                                           pushConstantInfo,
                                                                           vPipelineBuffers,
                                                                           (width + 31) / 32,
                                                                           (height + 31) / 32,
                                                                           1);

    ret = qCalculateNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buffer = inputBuffer;
    pipelineNodeInput.bufferSize = bufferSize;

    PipelineNodeBuffer pipelineNodeQInput;
    pipelineNodeQInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeQInput.buffer = qBuffer->GetBuffer();
    pipelineNodeQInput.bufferSize = bufferSize;

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buffer = outputBuffer;
    pipelineNodeOutput.bufferSize = bufferSize;

    std::vector<PipelineNodeBuffer> hPipelineBuffers;
    hPipelineBuffers.push_back(qPipelineNodeInput);
    hPipelineBuffers.push_back(pipelineNodeQInput);
    hPipelineBuffers.push_back(pipelineNodeOutput);

    const auto paletteKnifeNode = std::make_shared<PipelineComputeGraphNode>(gpuCtx,
                                                                             "PaletteKnife",
                                                                             SHADER(palette_knife.comp.glsl.spv),
                                                                             pushConstantInfo,
                                                                             hPipelineBuffers,
                                                                             (width + 31) / 32,
                                                                             (height + 31) / 32,
                                                                             1);

    ret = paletteKnifeNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }


    paletteKnifeNode->AddDependenceNode(qCalculateNode);
    computeGraph->AddComputeGraphNode(paletteKnifeNode);

    return computeGraph->Compute();
}


void PaletteKnifeFilter::Destroy() {
    computeGraph->Destroy();
    qBuffer->Destroy();
}
