//
// Created by neo on 2025/3/11.
//

#include "PPaletteKnifeFilter.h"

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

//std::vector<float> PPaletteKnifeFilter::CalculateWeights() {
//    std::vector<float> weights(2 * MAX_RADIUS + 1);
//    float sum = 0.0f;
//    constexpr float sigma = static_cast<float>(MAX_RADIUS) / 2.0f;
//
//    for (int i = 0; i <= 2 * MAX_RADIUS; ++i) {
//        const int x = i - MAX_RADIUS;
//        weights[i] = exp(-x * x / (2 * sigma * sigma));
//        sum += weights[i];
//    }
//
//    for (auto &w: weights) w /= sum;
//    return weights;
//}

VkResult PPaletteKnifeFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
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
    pushConstantInfo.size = sizeof(PPaletteKnifeFilterParams);
    pushConstantInfo.data = &this->paletteKnifeFilterParams;

    std::vector<uint32_t> queueFamilyIndices;
    queueFamilyIndices.push_back(0);
    const VkPhysicalDeviceMemoryProperties memoryProperties = gpuCtx->GetMemoryProperties();



    PipelineNodeBuffer vPipelineNodeInput;
    vPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    vPipelineNodeInput.buffer = inputBuffer;
    vPipelineNodeInput.bufferSize = bufferSize;



    VkBuffer qStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory qStorageBufferMemory = VK_NULL_HANDLE;

    VkGPUHelper::CreateStorageBufferAndBindMem(
        gpuCtx->GetCurrentDevice(),
        bufferSize,
        queueFamilyIndices,
        &memoryProperties,
        &qStorageBuffer,
        &qStorageBufferMemory
    );
    //
    PipelineNodeBuffer vPipelineNodeOutput;
    vPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    vPipelineNodeOutput.buffer = qStorageBuffer;
    vPipelineNodeOutput.bufferSize = bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(vPipelineNodeInput);
    vPipelineBuffers.push_back(vPipelineNodeOutput);
    //vPipelineBuffers.push_back(pipelineNodeWeightInput);

    const auto gaussianVerticalNode = std::make_shared<PipelineComputeGraphNode>(gpuCtx,
        "gaussianVerticalBlur",
        SHADER(palette_q.comp.glsl.spv),
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


    if (0) {
    
    
        computeGraph->AddComputeGraphNode(gaussianVerticalNode);

        return computeGraph->Compute();
    
    
    }




    PipelineNodeBuffer hPipelineNodeInput;
    hPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    hPipelineNodeInput.buffer = inputBuffer;
    hPipelineNodeInput.bufferSize = bufferSize;

    PipelineNodeBuffer hPipelineNodeQInput;
    hPipelineNodeQInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    hPipelineNodeQInput.buffer = qStorageBuffer;
    hPipelineNodeQInput.bufferSize = bufferSize;


    PipelineNodeBuffer hPipelineNodeOutput;
    hPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    hPipelineNodeOutput.buffer = outputBuffer;
    hPipelineNodeOutput.bufferSize = bufferSize;

    std::vector<PipelineNodeBuffer> hPipelineBuffers;
    hPipelineBuffers.push_back(hPipelineNodeInput);
    hPipelineBuffers.push_back(hPipelineNodeQInput);
    hPipelineBuffers.push_back(hPipelineNodeOutput);
  //  hPipelineBuffers.push_back(pipelineNodeWeightInput);

    const auto gaussianHorizontalNode = std::make_shared<PipelineComputeGraphNode>(gpuCtx,
        "gaussianHorizontalBlur",
        SHADER(palette_knife.comp.glsl.spv),
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


    gaussianHorizontalNode->AddDependenceNode(gaussianVerticalNode);
    computeGraph->AddComputeGraphNode(gaussianHorizontalNode);

    return computeGraph->Compute();
}



void PPaletteKnifeFilter::Destroy() {


    //vkFreeMemory(
    //
    //
    //);
}
