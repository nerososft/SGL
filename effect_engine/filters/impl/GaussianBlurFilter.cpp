//
// Created by neo on 2025/3/11.
//

#include "GaussianBlurFilter.h"

#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/compute_graph/BufferCopyComputeGraphNode.h"
#include "effect_engine/gpu/compute_graph/PipelineComputeGraphNode.h"

VkResult GaussianBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
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
        std::cout << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(GaussianBlurFilterParams);
    pushConstantInfo.data = &this->blurFilterParams;

    PipelineNodeInput vPipelineNodeInput;
    vPipelineNodeInput.buffer = inputBuffer;
    vPipelineNodeInput.bufferSize = bufferSize;

    PipelineNodeOutput vPipelineNodeOutput;
    vPipelineNodeOutput.buffer = outputBuffer;
    vPipelineNodeOutput.bufferSize = bufferSize;
    const auto gaussianVerticalNode = std::make_shared<PipelineComputeGraphNode>(gpuCtx,
        "gaussianVerticalBlur",
        "../../shader_compiled/vertical_blur.comp.glsl.spv",
        pushConstantInfo,
        vPipelineNodeInput,
        vPipelineNodeOutput,
        (width + 15) / 16,
        (height + 15) / 16,
        1);

    ret = gaussianVerticalNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PipelineNodeInput hPipelineNodeInput;
    hPipelineNodeInput.buffer = outputBuffer;
    hPipelineNodeInput.bufferSize = bufferSize;

    PipelineNodeOutput hPipelineNodeOutput;
    hPipelineNodeOutput.buffer = inputBuffer;
    hPipelineNodeOutput.bufferSize = bufferSize;
    const auto gaussianHorizontalNode = std::make_shared<PipelineComputeGraphNode>(gpuCtx,
        "gaussianHorizontalBlur",
        "../../shader_compiled/horizontal_blur.comp.glsl.spv",
        pushConstantInfo,
        hPipelineNodeInput,
        hPipelineNodeOutput,
        (width + 15) / 16,
        (height + 15) / 16,
        1);

    ret = gaussianHorizontalNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
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

void GaussianBlurFilter::Destroy() {
}
