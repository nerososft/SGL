//
// Created by neo on 2025/3/27.
//

#include "FastGaussianBlurFilter.h"

#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "ScaleFilter.h"
#include "effect_engine/gpu/VkGPUBuffer.h"
#include "effect_engine/gpu/compute_graph/BufferCopyNode.h"
#include "effect_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "effect_engine/log/Log.h"

VkResult FastGaussianBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
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

    const uint32_t sigma = this->blurFilterParams.radius / 3;
    uint32_t targetWidth = this->blurFilterParams.imageSize.width / sigma;
    uint32_t targetHeight = this->blurFilterParams.imageSize.height / sigma;
    uint32_t newRadius = this->blurFilterParams.radius / sigma;

    const auto scaleDownBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    const VkDeviceSize scaleDownBufferSize = targetWidth * targetWidth * this->blurFilterParams.imageSize.channels;
    scaleDownBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_LOCAL, scaleDownBufferSize);

    scaleParams.imageSize.width = this->blurFilterParams.imageSize.width;
    scaleParams.imageSize.height = this->blurFilterParams.imageSize.height;
    scaleParams.imageSize.channels = this->blurFilterParams.imageSize.channels;
    scaleParams.imageSize.bytesPerLine = this->blurFilterParams.imageSize.bytesPerLine;
    scaleParams.targetWidth = targetWidth;
    scaleParams.targetHeight = targetHeight;

    PushConstantInfo scaleDownPushConstantInfo;
    scaleDownPushConstantInfo.size = sizeof(ScaleFilterParams);
    scaleDownPushConstantInfo.data = &this->scaleParams;

    PipelineNodeBuffer scaleDownPipelineNodeInput;
    scaleDownPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    scaleDownPipelineNodeInput.buffer = inputBuffer;
    scaleDownPipelineNodeInput.bufferSize = bufferSize;

    PipelineNodeBuffer scaleDownPipelineNodeOutput;
    scaleDownPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    scaleDownPipelineNodeOutput.buffer = scaleDownBuffer->GetBuffer();
    scaleDownPipelineNodeOutput.bufferSize = scaleDownBufferSize;

    std::vector<PipelineNodeBuffer> scaleDownPipelineBuffers;
    scaleDownPipelineBuffers.push_back(scaleDownPipelineNodeInput);
    scaleDownPipelineBuffers.push_back(scaleDownPipelineNodeOutput);

    const auto scaleDownNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                     "ScaleDown",
                                                                     SHADER(scale.comp.glsl.spv),
                                                                     (targetWidth + 31) / 32,
                                                                     (targetHeight + 31) / 32,
                                                                     1);
    scaleDownNode->AddComputeElement({
        .pushConstantInfo = scaleDownPushConstantInfo,
        .buffers = scaleDownPipelineBuffers
    });

    ret = scaleDownNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    // TODO: blur for scaled buffer

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(FastGaussianBlurFilterParams);
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

    const auto gaussianVerticalNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                            "OldGaussianVerticalBlur",
                                                                            SHADER(vertical_blur_old.comp.glsl.spv),
                                                                            (width + 31) / 32,
                                                                            (height + 31) / 32,
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

    const auto gaussianHorizontalNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                              "OldGaussianHorizontalBlur",
                                                                              SHADER(horizontal_blur_old.comp.glsl.spv),
                                                                              (width + 31) / 32,
                                                                              (height + 31) / 32,
                                                                              1);
    gaussianHorizontalNode->AddComputeElement({
        .pushConstantInfo = pushConstantInfo,
        .buffers = hPipelineBuffers
    });

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
    const auto copyBufferNode = std::make_shared<BufferCopyNode>(gpuCtx,
                                                                 "CopyBufferToOutputBuffer",
                                                                 srcBufferInfo,
                                                                 dstBufferInfo);
    copyBufferNode->CreateComputeGraphNode();

    copyBufferNode->AddDependenceNode(gaussianVerticalNode);
    copyBufferNode->AddDependenceNode(gaussianHorizontalNode);
    computeGraph->AddComputeGraphNode(copyBufferNode);

    return computeGraph->Compute();
}

void FastGaussianBlurFilter::Destroy() {
    computeGraph->Destroy();
}
