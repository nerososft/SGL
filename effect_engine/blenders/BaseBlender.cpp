//
// Created by neo on 2025/3/14.
//

#include "BaseBlender.h"

#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/gpu/compute_graph/BufferCopyNode.h"
#include "effect_engine/gpu/compute_graph/ComputeGraph.h"
#include "effect_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "effect_engine/log/Log.h"

VkResult BaseBlender::DoApply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                              const std::string &name,
                              const BlendImageInfo &baseImageInfo,
                              const BlendImageInfo &blendImageInfo,
                              const VkBuffer outputBuffer,
                              const BasicBlenderParams &blenderParams,
                              uint32_t workGroupX,
                              uint32_t workGroupY,
                              uint32_t workGroupZ) {
    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    VkResult ret = this->computeGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    BufferCopyNodeBufferInfo srcBufferInfo;
    srcBufferInfo.buffer = baseImageInfo.storageBuffer;
    srcBufferInfo.bufferSize = baseImageInfo.bufferSize;
    BufferCopyNodeBufferInfo dstBufferInfo;
    dstBufferInfo.buffer = outputBuffer;
    dstBufferInfo.bufferSize = baseImageInfo.bufferSize;
    const auto copyBufferNode = std::make_shared<BufferCopyNode>(gpuCtx,
                                                                             "CopyBufferToOutputBuffer",
                                                                             srcBufferInfo,
                                                                             dstBufferInfo);

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = blenderParams.paramsSize;
    pushConstantInfo.data = blenderParams.paramsData;

    PipelineNodeBuffer pipelineNodeInput0;
    pipelineNodeInput0.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput0.buffer = baseImageInfo.storageBuffer;
    pipelineNodeInput0.bufferSize = baseImageInfo.bufferSize;

    PipelineNodeBuffer pipelineNodeInput1;
    pipelineNodeInput1.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput1.buffer = blendImageInfo.storageBuffer;
    pipelineNodeInput1.bufferSize = blendImageInfo.bufferSize;

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buffer = outputBuffer;
    pipelineNodeOutput.bufferSize = baseImageInfo.bufferSize;

    std::vector<PipelineNodeBuffer> pipelineBuffers;
    pipelineBuffers.push_back(pipelineNodeInput0);
    pipelineBuffers.push_back(pipelineNodeInput1);
    pipelineBuffers.push_back(pipelineNodeOutput);
    const auto blendNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                      name,
                                                                      blenderParams.shaderPath,
                                                                      pushConstantInfo,
                                                                      pipelineBuffers,
                                                                      workGroupX,
                                                                      workGroupY,
                                                                      workGroupZ);
    ret = blendNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    blendNode->AddDependenceNode(copyBufferNode);
    computeGraph->AddComputeGraphNode(blendNode);

    return computeGraph->Compute();
}

VkResult BaseBlender::DoApply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                              const std::string &name,
                              const BlendImageInfo &baseImageInfo,
                              const BlendImageInfo &blendImageInfo,
                              const VkBuffer outputBuffer,
                              const BasicBlenderParams &blenderParams) {
    return DoApply(gpuCtx,
                   name,
                   baseImageInfo,
                   blendImageInfo,
                   outputBuffer,
                   blenderParams,
                   (blendImageInfo.width + 31) / 32,
                   (blendImageInfo.height + 31) / 32,
                   1);
}

VkResult BaseBlender::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                            BlendImageInfo baseImageInfo,
                            BlendImageInfo blendImageInfo,
                            VkBuffer outputBuffer) {
    return VK_SUCCESS;
}

void BaseBlender::Destroy() {
}
