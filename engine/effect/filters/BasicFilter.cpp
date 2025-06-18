//
// Created by 1234 on 2025/3/6.
//

#include "BasicFilter.h"

#include <iostream>
#include <ostream>
#ifdef OS_OPEN_HARMONY
#include <core/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "core/gpu/VkGPUComputePipeline.h"
#include "core/gpu/VkGPUHelper.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"

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
    pipelineNodeInput.buf.buffer = inputBuffer;
    pipelineNodeInput.buf.bufferSize = bufferSize;

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buf.buffer = outputBuffer;
    pipelineNodeOutput.buf.bufferSize = bufferSize;

    std::vector<PipelineNodeBuffer> pipelineBuffers;
    pipelineBuffers.push_back(pipelineNodeInput);
    pipelineBuffers.push_back(pipelineNodeOutput);

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));

    const auto node = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                            name,
                                                            filterParams.shaderPath,
                                                            pushConstantInfo.size,
                                                            descriptorSetLayoutBindings,
                                                            workGroupX,
                                                            workGroupY,
                                                            workGroupZ);
    ret = node->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    node->AddComputeElement({
        .pushConstantInfo = pushConstantInfo,
        .buffers = pipelineBuffers
    });

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
    if (computeGraph != nullptr) {
        computeGraph->Destroy();
        computeGraph = nullptr;
    }
}
