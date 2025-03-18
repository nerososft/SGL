//
// Created by Jun Li on 2025/3/13.
//

#include "AverageBlurFilter.h"

#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/config.h"
#include "effect_engine/gpu/compute_graph/BufferCopyComputeGraphNode.h"
#include "effect_engine/gpu/compute_graph/PipelineComputeGraphNode.h"
#include "effect_engine/log/Log.h"

VkResult AverageBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                  const VkDeviceSize bufferSize,
                                  const uint32_t width,
                                  const uint32_t height,
                                  const VkBuffer inputBuffer,
                                  const VkBuffer outputBuffer) {
    BasicFilterParams params;
    blurParams.imageSize.width = width;
    blurParams.imageSize.height = height;
    blurParams.imageSize.channels = 4;
    blurParams.imageSize.bytesPerLine = width * 4;

    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    VkResult ret = this->computeGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(AverageBlurFilterParams);
    pushConstantInfo.data = &this->blurParams;

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
        "AverageVerticalBlur",
        SHADER(average_vblur.comp.glsl.spv),
        pushConstantInfo,
        vPipelineBuffers,
        width,
        (height + 255) / 256,
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
        "AverageHorizontalBlur",
        SHADER(average_hblur.comp.glsl.spv),
        pushConstantInfo,
        hPipelineBuffers,
        (width + 255) / 256,
        height,
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

void AverageBlurFilter::Destroy() {
}
