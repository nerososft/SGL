//
// Created by 1234 on 2025/3/6.
//

#include "BlackWhiteFilter.h"

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
    pipelineNodeInput.buffer = inputImageInfo[0].storageBuffer;
    pipelineNodeInput.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer pipelineNodeWInput;
    pipelineNodeWInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeWInput.buffer = weightBuffer->GetBuffer();
    pipelineNodeWInput.bufferSize = wSize * sizeof(float);

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buffer = outputImageInfo[0].storageBuffer;
    pipelineNodeOutput.bufferSize = outputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(pipelineNodeInput);
    vPipelineBuffers.push_back(pipelineNodeWInput);
    vPipelineBuffers.push_back(pipelineNodeOutput);

    const auto kCalculateNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                      "BlackWhite",
                                                                      SHADER(black_white.comp.glsl.spv),
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
