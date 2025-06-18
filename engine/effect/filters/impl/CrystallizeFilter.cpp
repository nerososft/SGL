#include "CrystallizeFilter.h"

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

VkResult CrystallizeFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                  const std::vector<FilterImageInfo> &inputImageInfo,
                                  const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->crystallizeFilterParams.imageSize.width = inputImageInfo[0].width;
    this->crystallizeFilterParams.imageSize.height = inputImageInfo[0].height;
    this->crystallizeFilterParams.imageSize.channels = 4;
    this->crystallizeFilterParams.imageSize.bytesPerLine = this->crystallizeFilterParams.imageSize.width * 4;

    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    VkResult ret = this->computeSubGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(CrystallizeFilterParams);
    pushConstantInfo.data = &this->crystallizeFilterParams;

    posxBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    posxBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, k_size * sizeof(float));
    posxBuffer->UploadData(posx, k_size * sizeof(float));

    posyBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    posyBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, k_size * sizeof(float));
    posyBuffer->UploadData(posy, k_size * sizeof(float));

    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
    pipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer pipelineNodeKInput;
    pipelineNodeKInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeKInput.buf.buffer = posxBuffer->GetBuffer();
    pipelineNodeKInput.buf.bufferSize = k_size * sizeof(float);

    PipelineNodeBuffer pipelineNodeK2Input;
    pipelineNodeK2Input.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeK2Input.buf.buffer = posyBuffer->GetBuffer();
    pipelineNodeK2Input.buf.bufferSize = k_size * sizeof(float);

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buf.buffer = outputImageInfo[0].storageBuffer;
    pipelineNodeOutput.buf.bufferSize = outputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(pipelineNodeInput);
    vPipelineBuffers.push_back(pipelineNodeKInput);
    vPipelineBuffers.push_back(pipelineNodeK2Input);
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
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));

    const auto kCalculateNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                      "crystallize",
                                                                      SHADER(crystallize.comp.glsl.spv),
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

void CrystallizeFilter::Destroy() {
    if (computeGraph != nullptr) {
        computeSubGraph->Destroy();
        computeGraph = nullptr;
    }
    if (posxBuffer != nullptr) {
        posxBuffer->Destroy();
        posxBuffer = nullptr;
    }
    if (posyBuffer != nullptr) {
        posyBuffer->Destroy();
        posyBuffer = nullptr;
    }
}
