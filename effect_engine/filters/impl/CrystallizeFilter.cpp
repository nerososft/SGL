#include "CrystallizeFilter.h"

#include "effect_engine/config.h"



#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUHelper.h"
#include "effect_engine/gpu/compute_graph/BufferCopyNode.h"
#include "effect_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "effect_engine/log/Log.h"
VkResult CrystallizeFilter::Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
    std::vector<FilterImageInfo> inputImageInfo,
    std::vector<FilterImageInfo> outputImageInfo){
    BasicFilterParams params;
    this->crystallizeFilterParams.imageSize.width = inputImageInfo[0].width;
    this->crystallizeFilterParams.imageSize.height = inputImageInfo[0].height;
    this->crystallizeFilterParams.imageSize.channels = 4;
    this->crystallizeFilterParams.imageSize.bytesPerLine = this->crystallizeFilterParams.imageSize.width * 4;
    //params.paramsSize = sizeof(customKernelFilterParams);
    //params.paramsData = &this->kFilterParams;
    //params.shaderPath = SHADER(customKernel.comp.glsl.spv);

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
    pipelineNodeInput.buffer = inputImageInfo[0].storageBuffer;
    pipelineNodeInput.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer pipelineNodeKInput;
    pipelineNodeKInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeKInput.buffer = posxBuffer->GetBuffer();
    pipelineNodeKInput.bufferSize = k_size * sizeof(float);

    PipelineNodeBuffer pipelineNodeK2Input;
    pipelineNodeK2Input.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeK2Input.buffer = posyBuffer->GetBuffer();
    pipelineNodeK2Input.bufferSize = k_size * sizeof(float);

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buffer = outputImageInfo[0].storageBuffer;
    pipelineNodeOutput.bufferSize = outputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(pipelineNodeInput);
    vPipelineBuffers.push_back(pipelineNodeKInput);
    vPipelineBuffers.push_back(pipelineNodeK2Input);
    vPipelineBuffers.push_back(pipelineNodeOutput);

    const auto kCalculateNode = std::make_shared<ComputePipelineNode>(gpuCtx,
        "crystallize",
        SHADER(crystallize.comp.glsl.spv),
        (inputImageInfo[0].width + 31) / 32,
        (inputImageInfo[0].height + 31) / 32,
        1);

    kCalculateNode->AddComputeElement({
    .pushConstantInfo = pushConstantInfo,
    .buffers = vPipelineBuffers
        });

    ret = kCalculateNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    computeSubGraph->AddComputeGraphNode(kCalculateNode);
    computeGraph->AddSubGraph(computeSubGraph);

    return computeGraph->Compute();

    /* ret = kCalculateNode->CreateComputeGraphNode();
     if (ret != VK_SUCCESS) {
         Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
         return ret;
     }

     computeGraph->AddComputeGraphNode(kCalculateNode);

     return computeGraph->Compute();*/
}

void CrystallizeFilter::Destroy() {
    Logger() << "CrystallizeFilter begin" << std::endl;
    posxBuffer->Destroy();
    posyBuffer->Destroy();
    BasicFilter::Destroy();
}