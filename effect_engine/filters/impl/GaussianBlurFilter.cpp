//
// Created by neo on 2025/3/11.
//

#include "GaussianBlurFilter.h"

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

std::vector<float> GaussianBlurFilter::CalculateWeights() {
    std::vector<float> weights(2 * MAX_RADIUS + 1);
    float sum = 0.0f;
    constexpr float sigma = static_cast<float>(MAX_RADIUS) / 2.0f;

    for (int i = 0; i <= 2 * MAX_RADIUS; ++i) {
        const int x = i - MAX_RADIUS;
        weights[i] = exp(-x * x / (2 * sigma * sigma));
        sum += weights[i];
    }

    for (auto &w: weights) w /= sum;
    return weights;
}

VkResult GaussianBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                   const VkDeviceSize bufferSize,
                                   const uint32_t width,
                                   const uint32_t height,
                                   const VkBuffer inputBuffer,
                                   const VkBuffer outputBuffer) {
    this->gpuCtx = gpuCtx;
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
    pushConstantInfo.size = sizeof(GaussianBlurFilterParams);
    pushConstantInfo.data = &this->blurFilterParams;

    std::vector<uint32_t> queueFamilyIndices;
    queueFamilyIndices.push_back(0);
    const VkPhysicalDeviceMemoryProperties memoryProperties = gpuCtx->GetMemoryProperties();

    std::vector<float> weights = CalculateWeights();
    const VkDeviceSize weightBufferSize = weights.size() * sizeof(float);
    ret = VkGPUHelper::CreateUniformBufferAndUploadData(gpuCtx->GetCurrentDevice(),
                                                        queueFamilyIndices,
                                                        &memoryProperties,
                                                        weightBufferSize,
                                                        &weightUniformBuffer,
                                                        &weightUniformBufferMemory,
                                                        weights.data());
    weights.clear();
    weights.resize(0);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create weight uniform buffer and upload data, err ="
                << string_VkResult(ret)
                << std::endl;
        return ret;
    }

    PipelineNodeBuffer pipelineNodeWeightInput;
    pipelineNodeWeightInput.type = PIPELINE_NODE_BUFFER_UNIFORM;
    pipelineNodeWeightInput.buffer = weightUniformBuffer;
    pipelineNodeWeightInput.bufferSize = weightBufferSize;


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
    vPipelineBuffers.push_back(pipelineNodeWeightInput);

    const auto gaussianVerticalNode = std::make_shared<PipelineComputeGraphNode>(gpuCtx,
        "gaussianVerticalBlur",
        SHADER(vertical_blur.comp.glsl.spv),
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
    hPipelineBuffers.push_back(pipelineNodeWeightInput);

    const auto gaussianHorizontalNode = std::make_shared<PipelineComputeGraphNode>(gpuCtx,
        "gaussianHorizontalBlur",
        SHADER(horizontal_blur.comp.glsl.spv),
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

void GaussianBlurFilter::Destroy() {
    computeGraph->Destroy();
    vkUnmapMemory(this->gpuCtx->GetCurrentDevice(), weightUniformBufferMemory);
    vkFreeMemory(this->gpuCtx->GetCurrentDevice(), weightUniformBufferMemory, nullptr);
    vkDestroyBuffer(this->gpuCtx->GetCurrentDevice(), weightUniformBuffer, nullptr);
}
