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

std::shared_ptr<IComputeGraphNode> FastGaussianBlurFilter::CreateScaleDownNode(
    const std::shared_ptr<VkGPUContext> &gpuCtx,
    const VkBuffer inputBuffer,
    const VkDeviceSize inputBufferSize,
    const VkBuffer outputBuffer,
    const VkDeviceSize outputBufferSize,
    const uint32_t targetWidth,
    const uint32_t targetHeight) {
    PushConstantInfo scaleDownPushConstantInfo;
    scaleDownPushConstantInfo.size = sizeof(ScaleFilterParams);
    scaleDownPushConstantInfo.data = &this->scaleDownParams;

    PipelineNodeBuffer scaleDownPipelineNodeInput;
    scaleDownPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    scaleDownPipelineNodeInput.buffer = inputBuffer;
    scaleDownPipelineNodeInput.bufferSize = inputBufferSize;

    PipelineNodeBuffer scaleDownPipelineNodeOutput;
    scaleDownPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    scaleDownPipelineNodeOutput.buffer = outputBuffer;
    scaleDownPipelineNodeOutput.bufferSize = outputBufferSize;

    std::vector<PipelineNodeBuffer> scaleDownPipelineBuffers;
    scaleDownPipelineBuffers.push_back(scaleDownPipelineNodeInput);
    scaleDownPipelineBuffers.push_back(scaleDownPipelineNodeOutput);

    const auto node = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                            "ScaleDown",
                                                            SHADER(scale.comp.glsl.spv),
                                                            (targetWidth + 31) / 32,
                                                            (targetHeight + 31) / 32,
                                                            1);
    node->AddComputeElement({
        .pushConstantInfo = scaleDownPushConstantInfo,
        .buffers = scaleDownPipelineBuffers
    });

    const VkResult ret = node->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return nullptr;
    }
    return node;
}

std::shared_ptr<IComputeGraphNode> FastGaussianBlurFilter::CreateVBlurNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                                                           const VkBuffer inputBuffer,
                                                                           const VkDeviceSize inputBufferSize,
                                                                           const VkBuffer outputBuffer,
                                                                           const VkDeviceSize outputBufferSize) {
    PushConstantInfo blurPushConstantInfo;
    blurPushConstantInfo.size = sizeof(FastGaussianBlurFilterParams);
    blurPushConstantInfo.data = &this->blurFilterParams;

    PipelineNodeBuffer vPipelineNodeInput;
    vPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    vPipelineNodeInput.buffer = inputBuffer;
    vPipelineNodeInput.bufferSize = inputBufferSize;

    PipelineNodeBuffer vPipelineNodeOutput;
    vPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    vPipelineNodeOutput.buffer = outputBuffer;
    vPipelineNodeOutput.bufferSize = outputBufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(vPipelineNodeInput);
    vPipelineBuffers.push_back(vPipelineNodeOutput);

    const auto gaussianVerticalNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                            "OldGaussianVerticalBlur",
                                                                            SHADER(vertical_blur_old.comp.glsl.spv),
                                                                            (this->blurFilterParams.imageSize.width +
                                                                             31) / 32,
                                                                            (this->blurFilterParams.imageSize.height +
                                                                             31) / 32,
                                                                            1);
    gaussianVerticalNode->AddComputeElement({
        .pushConstantInfo = blurPushConstantInfo,
        .buffers = vPipelineBuffers
    });

    const VkResult ret = gaussianVerticalNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return nullptr;
    }
    return gaussianVerticalNode;
}

std::shared_ptr<IComputeGraphNode> FastGaussianBlurFilter::CreateHBlurNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                                                           const VkBuffer inputBuffer,
                                                                           const VkDeviceSize inputBufferSize,
                                                                           const VkBuffer outputBuffer,
                                                                           const VkDeviceSize outputBufferSize) {
    PushConstantInfo blurPushConstantInfo;
    blurPushConstantInfo.size = sizeof(FastGaussianBlurFilterParams);
    blurPushConstantInfo.data = &this->blurFilterParams;

    PipelineNodeBuffer hPipelineNodeInput;
    hPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    hPipelineNodeInput.buffer = inputBuffer;
    hPipelineNodeInput.bufferSize = inputBufferSize;

    PipelineNodeBuffer hPipelineNodeOutput;
    hPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    hPipelineNodeOutput.buffer = outputBuffer;
    hPipelineNodeOutput.bufferSize = outputBufferSize;

    std::vector<PipelineNodeBuffer> hPipelineBuffers;
    hPipelineBuffers.push_back(hPipelineNodeInput);
    hPipelineBuffers.push_back(hPipelineNodeOutput);

    const auto gaussianHorizontalNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                              "OldGaussianHorizontalBlur",
                                                                              SHADER(horizontal_blur_old.comp.glsl.spv),
                                                                              (this->blurFilterParams.imageSize.width +
                                                                               31) / 32,
                                                                              (this->blurFilterParams.imageSize.height +
                                                                               31) / 32,
                                                                              1);
    gaussianHorizontalNode->AddComputeElement({
        .pushConstantInfo = blurPushConstantInfo,
        .buffers = hPipelineBuffers
    });

    const VkResult ret = gaussianHorizontalNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return nullptr;
    }
    return gaussianHorizontalNode;
}

std::shared_ptr<IComputeGraphNode> FastGaussianBlurFilter::CreateScaleUpNode(
    const std::shared_ptr<VkGPUContext> &gpuCtx,
    const VkBuffer inputBuffer,
    const VkDeviceSize inputBufferSize,
    const VkBuffer outputBuffer,
    const VkDeviceSize outputBufferSize,
    const uint32_t targetWidth,
    const uint32_t targetHeight) {
    PushConstantInfo scaleUpPushConstantInfo;
    scaleUpPushConstantInfo.size = sizeof(ScaleFilterParams);
    scaleUpPushConstantInfo.data = &this->scaleUpParams;

    PipelineNodeBuffer scaleUpPipelineNodeInput;
    scaleUpPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    scaleUpPipelineNodeInput.buffer = inputBuffer;
    scaleUpPipelineNodeInput.bufferSize = inputBufferSize;

    PipelineNodeBuffer scaleUpPipelineNodeOutput;
    scaleUpPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    scaleUpPipelineNodeOutput.buffer = outputBuffer;
    scaleUpPipelineNodeOutput.bufferSize = outputBufferSize;

    std::vector<PipelineNodeBuffer> scaleUpPipelineBuffers;
    scaleUpPipelineBuffers.push_back(scaleUpPipelineNodeInput);
    scaleUpPipelineBuffers.push_back(scaleUpPipelineNodeOutput);

    const auto node = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                            "ScaleUp",
                                                            SHADER(scale.comp.glsl.spv),
                                                            (targetWidth + 31) / 32,
                                                            (targetHeight + 31) / 32,
                                                            1);
    node->AddComputeElement({
        .pushConstantInfo = scaleUpPushConstantInfo,
        .buffers = scaleUpPipelineBuffers
    });

    const VkResult ret = node->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return nullptr;
    }
    return node;
}

VkResult FastGaussianBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                       const std::vector<FilterImageInfo> &inputImageInfo,
                                       const std::vector<FilterImageInfo> &outputImageInfo) {
    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    VkResult ret = this->computeSubGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }
    uint32_t ratio = this->blurFilterParams.radius / 3;
    ratio = ratio > 4 ? 4 : ratio;
    ratio = ratio < 2 ? 2 : ratio;
    Logger() << "Downsampling ratio: " << ratio << std::endl;
    const uint32_t targetWidth = inputImageInfo[0].width / ratio;
    const uint32_t targetHeight = inputImageInfo[0].height / ratio;
    const int newRadius = this->blurFilterParams.radius / ratio;

    scaleDownBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    const VkDeviceSize scaleDownBufferSize = targetWidth * targetHeight * 4;
    ret = scaleDownBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, scaleDownBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to allocate scale buffer memory, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    scaleDownParams.imageSize.width = inputImageInfo[0].width;
    scaleDownParams.imageSize.height = inputImageInfo[0].height;
    scaleDownParams.imageSize.channels = 4;
    scaleDownParams.imageSize.bytesPerLine = inputImageInfo[0].width * 4;
    scaleDownParams.targetWidth = targetWidth;
    scaleDownParams.targetHeight = targetHeight;
    const std::shared_ptr<IComputeGraphNode> scaleDownNode = CreateScaleDownNode(gpuCtx,
        inputImageInfo[0].storageBuffer,
        inputImageInfo[0].bufferSize,
        scaleDownBuffer->GetBuffer(),
        scaleDownBufferSize,
        targetWidth,
        targetHeight);

    this->blurFilterParams.imageSize.width = targetWidth;
    this->blurFilterParams.imageSize.height = targetHeight;
    this->blurFilterParams.imageSize.channels = 4;
    this->blurFilterParams.imageSize.bytesPerLine = targetWidth * 4;
    this->blurFilterParams.radius = newRadius;
    scaleDownBlurBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    ret = scaleDownBlurBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, scaleDownBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to allocate blur buffer memory, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    const std::shared_ptr<IComputeGraphNode> vBlurNode = CreateVBlurNode(gpuCtx,
                                                                         scaleDownBuffer->GetBuffer(),
                                                                         scaleDownBufferSize,
                                                                         scaleDownBlurBuffer->GetBuffer(),
                                                                         scaleDownBufferSize);
    const std::shared_ptr<IComputeGraphNode> hBlurNode = CreateHBlurNode(gpuCtx,
                                                                         scaleDownBlurBuffer->GetBuffer(),
                                                                         scaleDownBufferSize,
                                                                         scaleDownBuffer->GetBuffer(),
                                                                         scaleDownBufferSize);

    scaleUpParams.imageSize.width = targetWidth;
    scaleUpParams.imageSize.height = targetHeight;
    scaleUpParams.imageSize.channels = 4;
    scaleUpParams.imageSize.bytesPerLine = targetWidth * 4;
    scaleUpParams.targetWidth = inputImageInfo[0].width;
    scaleUpParams.targetHeight = inputImageInfo[0].height;
    const std::shared_ptr<IComputeGraphNode> scaleUpNode = CreateScaleUpNode(gpuCtx,
                                                                             scaleDownBuffer->GetBuffer(),
                                                                             scaleDownBufferSize,
                                                                             outputImageInfo[0].storageBuffer,
                                                                             outputImageInfo[0].bufferSize,
                                                                             outputImageInfo[0].width,
                                                                             outputImageInfo[0].height);

    vBlurNode->AddDependenceNode(scaleDownNode);
    hBlurNode->AddDependenceNode(vBlurNode);
    scaleUpNode->AddDependenceNode(hBlurNode);
    this->computeSubGraph->AddComputeGraphNode(scaleUpNode);
    this->computeGraph->AddSubGraph(computeSubGraph);

    return computeGraph->Compute();
}

void FastGaussianBlurFilter::Destroy() {
    computeGraph->Destroy();
}
