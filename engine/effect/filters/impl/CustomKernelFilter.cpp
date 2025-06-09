//
// Created by 1234 on 2025/3/6.
//

#include "CustomKernelFilter.h"

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

VkResult CustomKernelFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                   const std::vector<FilterImageInfo> &inputImageInfo,
                                   const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->kFilterParams.imageSize.width = inputImageInfo[0].width;
    this->kFilterParams.imageSize.height = inputImageInfo[0].height;
    this->kFilterParams.imageSize.channels = 4;
    this->kFilterParams.imageSize.bytesPerLine = this->kFilterParams.imageSize.width * 4;

    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);

    VkResult ret = this->computeSubGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(customKernelFilterParams);
    pushConstantInfo.data = &this->kFilterParams;


    //int k[25] = { 0 , 0 ,0 ,0 ,0,
    //		0 , 0 ,-1 ,0 ,0,
    //		0 , -1 ,5 ,-1 ,0,
    //		0 , 0 ,-1 ,0 ,0,
    //	   0 , 0 ,0 ,0 ,0 };

    kBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    kBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, k_size * sizeof(int));
    kBuffer->UploadData(k, k_size * sizeof(int));

    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
    pipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer pipelineNodeKInput;
    pipelineNodeKInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeKInput.buf.buffer = kBuffer->GetBuffer();
    pipelineNodeKInput.buf.bufferSize = k_size * sizeof(int);

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buf.buffer = outputImageInfo[0].storageBuffer;
    pipelineNodeOutput.buf.bufferSize = outputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(pipelineNodeInput);
    vPipelineBuffers.push_back(pipelineNodeKInput);
    vPipelineBuffers.push_back(pipelineNodeOutput);

    const auto kCalculateNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                      "kCalculate",
                                                                      SHADER(custom_kernel.comp.glsl.spv),
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

void CustomKernelFilter::Destroy() {
    if (computeGraph != nullptr) {
        computeGraph->Destroy();
        computeGraph = nullptr;
    }
    if (kBuffer != nullptr) {
        kBuffer->Destroy();
        kBuffer = nullptr;
    }
}
