//
// Created by neo on 2025/3/11.
//

#include "OldGaussianBlurFloatFilter.h"

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

VkResult OldGaussianBlurFloatFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                           const std::vector<FilterImageInfo> &inputImageInfo,
                                           const std::vector<FilterImageInfo> &outputImageInfo) {
    this->blurFilterParams.imageSize.width = inputImageInfo[0].width;
    this->blurFilterParams.imageSize.height = inputImageInfo[0].height;
    this->blurFilterParams.imageSize.channels = 4;
    this->blurFilterParams.imageSize.bytesPerLine = this->blurFilterParams.imageSize.width * 4;

    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    VkResult ret = this->computeSubGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(OldGaussianBlurFloatFilterParams);
    pushConstantInfo.data = &this->blurFilterParams;

    PipelineNodeBuffer vPipelineNodeInput;
    vPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    vPipelineNodeInput.buffer = inputImageInfo[0].storageBuffer;
    vPipelineNodeInput.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer vPipelineNodeOutput;
    vPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    vPipelineNodeOutput.buffer = outputImageInfo[0].storageBuffer;
    vPipelineNodeOutput.bufferSize = outputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(vPipelineNodeInput);
    vPipelineBuffers.push_back(vPipelineNodeOutput);

    const auto gaussianVerticalNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                            "OldGaussianVerticalBlurFloat",
                                                                            SHADER(
                                                                                vertical_blur_old_float.comp.glsl.spv),
                                                                            (inputImageInfo[0].width + 31) / 32,
                                                                            (inputImageInfo[0].height + 31) / 32,
                                                                            1);
    gaussianVerticalNode->AddComputeElement({
        .pushConstantInfo = pushConstantInfo,
        .buffers = vPipelineBuffers
    });

    ret = gaussianVerticalNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    //PipelineNodeBuffer hPipelineNodeInput;
    //hPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    //hPipelineNodeInput.buffer = outputBuffer;
    //hPipelineNodeInput.bufferSize = bufferSize;

    //PipelineNodeBuffer hPipelineNodeOutput;
    //hPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    //hPipelineNodeOutput.buffer = inputBuffer;
    //hPipelineNodeOutput.bufferSize = bufferSize;

    //std::vector<PipelineNodeBuffer> hPipelineBuffers;
    //hPipelineBuffers.push_back(hPipelineNodeInput);
    //hPipelineBuffers.push_back(hPipelineNodeOutput);

    //const auto gaussianHorizontalNode = std::make_shared<ComputePipelineNode>(gpuCtx,
    //                                                                          "OldGaussianHorizontalBlurFloat",
    //                                                                          SHADER(horizontal_blur_old_float.comp.glsl.spv),
    //                                                                          (width + 31) / 32,
    //                                                                          (height + 31) / 32,
    //                                                                          1);
    //gaussianHorizontalNode->AddComputeElement({
    //    .pushConstantInfo = pushConstantInfo,
    //    .buffers = hPipelineBuffers
    //});

    //ret = gaussianHorizontalNode->CreateComputeGraphNode();
    //if (ret != VK_SUCCESS) {
    //    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
    //    return ret;
    //}

    //BufferCopyNodeBufferInfo srcBufferInfo;
    //srcBufferInfo.buffer = inputBuffer;
    //srcBufferInfo.bufferSize = bufferSize;
    //BufferCopyNodeBufferInfo dstBufferInfo;
    //dstBufferInfo.buffer = outputBuffer;
    //dstBufferInfo.bufferSize = bufferSize;
    //const auto copyBufferNode = std::make_shared<BufferCopyNode>(gpuCtx,
    //                                                             "CopyBufferToOutputBuffer",
    //                                                             srcBufferInfo,
    //                                                             dstBufferInfo);
    //copyBufferNode->CreateComputeGraphNode();

    //copyBufferNode->AddDependenceNode(gaussianVerticalNode);
    //copyBufferNode->AddDependenceNode(gaussianHorizontalNode);
    //computeSubGraph->AddComputeGraphNode(copyBufferNode);
    //computeGraph->AddSubGraph(computeSubGraph);

    return computeGraph->Compute();
}

void OldGaussianBlurFloatFilter::Destroy() {
    computeGraph->Destroy();
}
