//
// Created by 1234 on 2025/3/6.
//

#include "BasicFilter.h"

#include <iostream>
#include <ostream>
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/gpu/VkGPUComputePipeline.h"
#include "effect_engine/gpu/compute_graph/PipelineComputeGraphNode.h"
#include "effect_engine/log/Log.h"

VkResult BasicFilter::DoApply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                              const std::string &name,
                              const VkDeviceSize bufferSize,
                              const VkBuffer inputBuffer,
                              const VkBuffer outputBuffer,
                              const BasicFilterParams &filterParams,
                              uint32_t workGroupX,
                              uint32_t workGroupY,
                              uint32_t workGroupZ) {
    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    VkResult ret = this->computeGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = filterParams.paramsSize;
    pushConstantInfo.data = filterParams.paramsData;

    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buffer = inputBuffer;
    pipelineNodeInput.bufferSize = bufferSize;

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buffer = outputBuffer;
    pipelineNodeOutput.bufferSize = bufferSize;

    std::vector<PipelineNodeBuffer> pipelineBuffers;
    pipelineBuffers.push_back(pipelineNodeInput);
    pipelineBuffers.push_back(pipelineNodeOutput);

    const auto grayNode = std::make_shared<PipelineComputeGraphNode>(gpuCtx,
                                                                     name,
                                                                     filterParams.shaderPath,
                                                                     pushConstantInfo,
                                                                     pipelineBuffers,
                                                                     workGroupX,
                                                                     workGroupY,
                                                                     workGroupZ);
    ret = grayNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }
    computeGraph->AddComputeGraphNode(grayNode);

    return computeGraph->Compute();
}

VkResult BasicFilter::DoApply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                              const std::string &name,
                              const VkDeviceSize bufferSize,
                              const uint32_t width,
                              const uint32_t height,
                              const VkBuffer inputBuffer,
                              const VkBuffer outputBuffer,
                              const BasicFilterParams &filterParams) {
    return DoApply(gpuCtx,
                   name,
                   bufferSize,
                   inputBuffer,
                   outputBuffer,
                   filterParams,
                   (width + 15) / 16,
                   (height + 15) / 16,
                   1);
}

VkResult BasicFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                            VkDeviceSize bufferSize,
                            uint32_t width,
                            uint32_t height,
                            VkBuffer inputBuffer,
                            VkBuffer outputBuffer) {
    return VK_SUCCESS;
}

void BasicFilter::Destroy() {
    computeGraph->Destroy();
}
