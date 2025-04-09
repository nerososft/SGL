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
#include "effect_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "effect_engine/log/Log.h"

VkResult BasicFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                            const std::string &name,
                            const VkDeviceSize bufferSize,
                            const VkBuffer inputBuffer,
                            const VkBuffer outputBuffer,
                            const BasicFilterParams &filterParams,
                            uint32_t workGroupX,
                            uint32_t workGroupY,
                            uint32_t workGroupZ) {
    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    VkResult ret = this->computeSubGraph->Init();
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

    const auto node = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                            name,
                                                            filterParams.shaderPath,
                                                            workGroupX,
                                                            workGroupY,
                                                            workGroupZ);
    node->AddComputeElement({
        .pushConstantInfo = pushConstantInfo,
        .buffers = pipelineBuffers
    });
    ret = node->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }
    computeSubGraph->AddComputeGraphNode(node);
    computeGraph->AddSubGraph(this->computeSubGraph);

    return computeGraph->Compute();
}

VkResult BasicFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                            const std::vector<FilterImageInfo> &inputImageInfo,
                            const std::vector<FilterImageInfo> &outputImageInfo) {
    return VK_SUCCESS;
}

void BasicFilter::Destroy() {
    if (computeGraph) {
        computeGraph->Destroy();
    }
}
