//
// Created by 1234 on 2025/3/6.
//

#include "BlackWhiteFilter.h"

#include "core/config.h"


#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <core/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "engine/effect/filters/BasicFilter.h"
#include "core/gpu/VkGPUHelper.h"
#include "core/gpu/compute_graph/BufferCopyNode.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"

VkResult BlackWhiteFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                 const std::vector<FilterImageInfo> &inputImageInfo,
                                 const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->wFilterParams.imageSize.width = inputImageInfo[0].width;
    this->wFilterParams.imageSize.height = inputImageInfo[0].height;
    this->wFilterParams.imageSize.channels = 4;
    this->wFilterParams.imageSize.bytesPerLine = this->wFilterParams.imageSize.width * 4;

    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);

    VkResult ret = this->computeSubGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(BlackWhiteFilterParams);
    pushConstantInfo.data = &this->wFilterParams;

    weightBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    weightBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, wSize * sizeof(float));
    weightBuffer->UploadData(weight, wSize * sizeof(float));

    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
    pipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer pipelineNodeWInput;
    pipelineNodeWInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeWInput.buf.buffer = weightBuffer->GetBuffer();
    pipelineNodeWInput.buf.bufferSize = wSize * sizeof(float);

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buf.buffer = outputImageInfo[0].storageBuffer;
    pipelineNodeOutput.buf.bufferSize = outputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(pipelineNodeInput);
    vPipelineBuffers.push_back(pipelineNodeWInput);
    vPipelineBuffers.push_back(pipelineNodeOutput);

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));

    const auto kCalculateNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                      "BlackWhite",
                                                                      SHADER(black_white.comp.glsl.spv),
                                                                      pushConstantInfo.size,
                                                                      descriptorSetLayoutBindings,
                                                                      (inputImageInfo[0].width + 31) / 32,
                                                                      (inputImageInfo[0].height + 31) / 32,
                                                                      1);

    ret = kCalculateNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    kCalculateNode->AddComputeElement({
        .pushConstantInfo = pushConstantInfo,
        .buffers = vPipelineBuffers
    });

    computeSubGraph->AddComputeGraphNode(kCalculateNode);
    computeGraph->AddSubGraph(computeSubGraph);

    return computeGraph->Compute();
}

void BlackWhiteFilter::Destroy() {
    if (computeGraph != nullptr) {
        computeGraph->Destroy();
        computeGraph = nullptr;
    }
    if (weightBuffer != nullptr) {
        weightBuffer->Destroy();
        weightBuffer = nullptr;
    }
}
