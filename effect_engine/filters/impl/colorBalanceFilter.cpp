//
// Created by 1234 on 2025/3/6.
//

#include "colorBalanceFilter.h"

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


VkResult colorBalanceFilter::Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
    std::vector<FilterImageInfo> inputImageInfo,
    std::vector<FilterImageInfo> outputImageInfo){
    BasicFilterParams params;
    this->bFilterParams.imageSize.width = inputImageInfo[0].width;
    this->bFilterParams.imageSize.height = inputImageInfo[0].height;
    this->bFilterParams.imageSize.channels = 4;
    this->bFilterParams.imageSize.bytesPerLine = this->bFilterParams.imageSize.width * 4;


    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);

    VkResult ret = this->computeSubGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(colorBalanceFilterParams);
    pushConstantInfo.data = &this->bFilterParams;



    PBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    PBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, pSize * sizeof(int) );


    adjustPBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    adjustPBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, adjustPSize * sizeof(float));
    //kBuffer->GetBuffer();
    PBuffer->UploadData(P, pSize * sizeof(int));
    adjustPBuffer->UploadData(adjustP , adjustPSize * sizeof(float) );

    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buffer = inputImageInfo[0].storageBuffer;
    pipelineNodeInput.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer pipelineNodeAdjustPInput;
    pipelineNodeAdjustPInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeAdjustPInput.buffer = adjustPBuffer->GetBuffer();
    pipelineNodeAdjustPInput.bufferSize = pSize * sizeof(float);

    PipelineNodeBuffer pipelineNodePInput;
    pipelineNodePInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodePInput.buffer = PBuffer->GetBuffer();
    pipelineNodePInput.bufferSize = pSize * sizeof(int);

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buffer = outputImageInfo[0].storageBuffer;
    pipelineNodeOutput.bufferSize = outputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(pipelineNodeInput);
    vPipelineBuffers.push_back(pipelineNodeAdjustPInput);
    vPipelineBuffers.push_back(pipelineNodePInput);
    vPipelineBuffers.push_back(pipelineNodeOutput);

    const auto kCalculateNode = std::make_shared<ComputePipelineNode>(gpuCtx,
        "colorBalance",
        SHADER(color_balance.comp.glsl.spv),
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
}

void colorBalanceFilter::Destroy() {
    Logger() << "customKernelFilter begin" << std::endl;
  this->computeGraph->Destroy();
  PBuffer->Destroy();
  adjustPBuffer->Destroy();
  //  BasicFilter::Destroy();
}
