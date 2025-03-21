//
// Created by neo on 2025/3/11.
//

#include "OldGaussianBlurFilter.h"

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

VkResult OldGaussianBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                      const VkDeviceSize bufferSize,
                                      const uint32_t width,
                                      const uint32_t height,
                                      const VkBuffer inputBuffer,
                                      const VkBuffer outputBuffer) {
    this->blurFilterParams.imageSize.width = width;
    this->blurFilterParams.imageSize.height = height;
    this->blurFilterParams.imageSize.channels = 4;
    this->blurFilterParams.imageSize.bytesPerLine = this->blurFilterParams.imageSize.width * 4;

    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    VkResult ret = this->computeGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(OldGaussianBlurFilterParams);
    pushConstantInfo.data = &this->blurFilterParams;

    PipelineNodeBuffer vPipelineNodeInput;
    vPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    vPipelineNodeInput.buffer = inputBuffer;
    vPipelineNodeInput.bufferSize = bufferSize;

    PipelineNodeBuffer vPipelineNodeOutput;
    vPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    vPipelineNodeOutput.buffer = outputBuffer;
    vPipelineNodeOutput.bufferSize = bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(vPipelineNodeInput);
    vPipelineBuffers.push_back(vPipelineNodeOutput);

    const auto gaussianVerticalNode = std::make_shared<PipelineComputeGraphNode>(gpuCtx,
        "OldGaussianVerticalBlur",
        SHADER(vertical_blur_old.comp.glsl.spv),
        pushConstantInfo,
        vPipelineBuffers,
        (width + 31) / 32,
        (height + 31) / 32,
        1);

    ret = gaussianVerticalNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PipelineNodeBuffer hPipelineNodeInput;
    hPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    hPipelineNodeInput.buffer = outputBuffer;
    hPipelineNodeInput.bufferSize = bufferSize;

    PipelineNodeBuffer hPipelineNodeOutput;
    hPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    hPipelineNodeOutput.buffer = inputBuffer;
    hPipelineNodeOutput.bufferSize = bufferSize;

    std::vector<PipelineNodeBuffer> hPipelineBuffers;
    hPipelineBuffers.push_back(hPipelineNodeInput);
    hPipelineBuffers.push_back(hPipelineNodeOutput);

    const auto gaussianHorizontalNode = std::make_shared<PipelineComputeGraphNode>(gpuCtx,
        "OldGaussianHorizontalBlur",
        SHADER(horizontal_blur_old.comp.glsl.spv),
        pushConstantInfo,
        hPipelineBuffers,
        (width + 31) / 32,
        (height + 31) / 32,
        1);

    ret = gaussianHorizontalNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    BufferCopyNodeBufferInfo srcBufferInfo;
    srcBufferInfo.buffer = inputBuffer;
    srcBufferInfo.bufferSize = bufferSize;
    BufferCopyNodeBufferInfo dstBufferInfo;
    dstBufferInfo.buffer = outputBuffer;
    dstBufferInfo.bufferSize = bufferSize;
    const auto copyBufferNode = std::make_shared<BufferCopyComputeGraphNode>(gpuCtx,
                                                                             "CopyBufferToOutputBuffer",
                                                                             srcBufferInfo,
                                                                             dstBufferInfo);
    copyBufferNode->CreateComputeGraphNode();

    copyBufferNode->AddDependenceNode(gaussianVerticalNode);
    copyBufferNode->AddDependenceNode(gaussianHorizontalNode);
    computeGraph->AddComputeGraphNode(copyBufferNode);

    return computeGraph->Compute();
}

void OldGaussianBlurFilter::Destroy() {
    computeGraph->Destroy();
}
