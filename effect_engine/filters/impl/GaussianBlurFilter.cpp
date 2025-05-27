//
// Created by neo on 2025/3/11.
//

#include "GaussianBlurFilter.h"

#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <gpu_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif
#include <cmath>

#include "gpu_engine/config.h"
#include "effect_engine/filters/BasicFilter.h"
#include "gpu_engine/gpu/VkGPUHelper.h"
#include "gpu_engine/gpu/compute_graph/BufferCopyNode.h"
#include "gpu_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "gpu_engine/log/Log.h"

std::vector<float> GaussianBlurFilter::CalculateWeights() {
    std::vector<float> weights(2 * MAX_RADIUS + 1);
    float sum = 0.0f;
    constexpr float sigma = static_cast<float>(MAX_RADIUS) / 2.0f;

    for (int i = 0; i <= 2 * MAX_RADIUS; ++i) {
        const int x = i - MAX_RADIUS;
        weights[i] = std::exp(-x * x / (2 * sigma * sigma));
        sum += weights[i];
    }

    for (auto &w: weights) w /= sum;
    return weights;
}

VkResult GaussianBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                   const std::vector<FilterImageInfo> &inputImageInfo,
                                   const std::vector<FilterImageInfo> &outputImageInfo) {
    this->gpuCtx = gpuCtx;
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
    pushConstantInfo.size = sizeof(GaussianBlurFilterParams);
    pushConstantInfo.data = &this->blurFilterParams;

    std::vector<float> weights = CalculateWeights();
    const VkDeviceSize weightBufferSize = weights.size() * sizeof(float);
    weightBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    weightBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, weightBufferSize);
    weightBuffer->UploadData(weights.data(), weightBufferSize);
    weights.clear();
    weights.resize(0);

    PipelineNodeBuffer pipelineNodeWeightInput;
    pipelineNodeWeightInput.type = PIPELINE_NODE_BUFFER_UNIFORM;
    pipelineNodeWeightInput.buf.buffer = weightBuffer->GetBuffer();
    pipelineNodeWeightInput.buf.bufferSize = weightBufferSize;

    PipelineNodeBuffer vPipelineNodeInput;
    vPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    vPipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
    vPipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer vPipelineNodeOutput;
    vPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    vPipelineNodeOutput.buf.buffer = outputImageInfo[0].storageBuffer;
    vPipelineNodeOutput.buf.bufferSize = outputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(vPipelineNodeInput);
    vPipelineBuffers.push_back(vPipelineNodeOutput);
    vPipelineBuffers.push_back(pipelineNodeWeightInput);

    const auto gaussianVerticalNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                            "gaussianVerticalBlur",
                                                                            SHADER(vertical_blur.comp.glsl.spv),
                                                                            inputImageInfo[0].width,
                                                                            (inputImageInfo[0].height + 255) / 256,
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
    hPipelineNodeInput.buf.buffer = outputImageInfo[0].storageBuffer;
    hPipelineNodeInput.buf.bufferSize = outputImageInfo[0].bufferSize;

    PipelineNodeBuffer hPipelineNodeOutput;
    hPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    hPipelineNodeOutput.buf.buffer = inputImageInfo[0].storageBuffer;
    hPipelineNodeOutput.buf.bufferSize = inputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> hPipelineBuffers;
    hPipelineBuffers.push_back(hPipelineNodeInput);
    hPipelineBuffers.push_back(hPipelineNodeOutput);
    hPipelineBuffers.push_back(pipelineNodeWeightInput);

    const auto gaussianHorizontalNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                              "gaussianHorizontalBlur",
                                                                              SHADER(horizontal_blur.comp.glsl.spv),
                                                                              (outputImageInfo[0].width + 255) / 256,
                                                                              outputImageInfo[0].height,
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
    srcBufferInfo.buffer = inputImageInfo[0].storageBuffer;
    srcBufferInfo.bufferSize = inputImageInfo[0].bufferSize;
    BufferCopyNodeBufferInfo dstBufferInfo;
    dstBufferInfo.buffer = outputImageInfo[0].storageBuffer;
    dstBufferInfo.bufferSize = outputImageInfo[0].bufferSize;
    const auto copyBufferNode = std::make_shared<BufferCopyNode>(gpuCtx,
                                                                 "CopyBufferToOutputBuffer",
                                                                 srcBufferInfo,
                                                                 dstBufferInfo);
    copyBufferNode->CreateComputeGraphNode();

    copyBufferNode->AddDependenceNode(gaussianVerticalNode);
    copyBufferNode->AddDependenceNode(gaussianHorizontalNode);
    computeSubGraph->AddComputeGraphNode(copyBufferNode);
    computeGraph->AddSubGraph(computeSubGraph);

    return computeGraph->Compute();
}

void GaussianBlurFilter::Destroy() {
    if (computeGraph != nullptr) {
        computeGraph->Destroy();
        computeGraph = nullptr;
    }
    if (weightBuffer != nullptr) {
        weightBuffer->Destroy();
        weightBuffer = nullptr;
    }
}
