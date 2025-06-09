//
// Created by neo on 2025/3/11.
//

#include "PaletteKnifeFilter.h"

#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <core/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif
#include "core/config.h"
#include "engine/effect/filters/BasicFilter.h"
#include "core/gpu/VkGPUHelper.h"
#include "core/gpu/compute_graph/BufferCopyNode.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"

VkResult PaletteKnifeFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                   const std::vector<FilterImageInfo> &inputImageInfo,
                                   const std::vector<FilterImageInfo> &outputImageInfo) {
    this->paletteKnifeFilterParams.imageSize.width = inputImageInfo[0].width;
    this->paletteKnifeFilterParams.imageSize.height = inputImageInfo[0].height;
    this->paletteKnifeFilterParams.imageSize.channels = 4;
    this->paletteKnifeFilterParams.imageSize.bytesPerLine = this->paletteKnifeFilterParams.imageSize.width * 4;

    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    VkResult ret = this->computeSubGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    Logger() << "PaletteKnifeFilter  radius " << this->paletteKnifeFilterParams.radius << std::endl;

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(PaletteKnifeFilterParams);
    pushConstantInfo.data = &this->paletteKnifeFilterParams;

    PipelineNodeBuffer qPipelineNodeInput;
    qPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    qPipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
    qPipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

    qBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    qBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_LOCAL, inputImageInfo[0].bufferSize);

    PipelineNodeBuffer qPipelineNodeOutput;
    qPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    qPipelineNodeOutput.buf.buffer = qBuffer->GetBuffer();
    qPipelineNodeOutput.buf.bufferSize = inputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> qPipelineBuffers;
    qPipelineBuffers.push_back(qPipelineNodeInput);
    qPipelineBuffers.push_back(qPipelineNodeOutput);

    const auto qCalculateNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                      "QCalculate",
                                                                      SHADER(palette_q.comp.glsl.spv),
                                                                      (inputImageInfo[0].width + 31) / 32,
                                                                      (inputImageInfo[0].height + 31) / 32,
                                                                      1);
    qCalculateNode->AddComputeElement({
        .pushConstantInfo = pushConstantInfo,
        .buffers = qPipelineBuffers
    });

    ret = qCalculateNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
    pipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer pipelineNodeQInput;
    pipelineNodeQInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeQInput.buf.buffer = qBuffer->GetBuffer();
    pipelineNodeQInput.buf.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buf.buffer = outputImageInfo[0].storageBuffer;
    pipelineNodeOutput.buf.bufferSize = outputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> pkPipelineBuffers;
    pkPipelineBuffers.push_back(qPipelineNodeInput);
    pkPipelineBuffers.push_back(pipelineNodeQInput);
    pkPipelineBuffers.push_back(pipelineNodeOutput);

    const auto paletteKnifeNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                        "PaletteKnife",
                                                                        SHADER(palette_knife.comp.glsl.spv),
                                                                        (outputImageInfo[0].width + 31) / 32,
                                                                        (outputImageInfo[0].height + 31) / 32,
                                                                        1);
    paletteKnifeNode->AddComputeElement({
        .pushConstantInfo = pushConstantInfo,
        .buffers = pkPipelineBuffers
    });

    ret = paletteKnifeNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }


    paletteKnifeNode->AddDependenceNode(qCalculateNode);
    computeSubGraph->AddComputeGraphNode(paletteKnifeNode);
    computeGraph->AddSubGraph(computeSubGraph);
    return computeGraph->Compute();
}

void PaletteKnifeFilter::Destroy() {
    if (computeGraph != nullptr) {
        computeGraph->Destroy();
        computeGraph = nullptr;
    }
    if (qBuffer != nullptr) {
        qBuffer->Destroy();
        qBuffer = nullptr;
    }
}
