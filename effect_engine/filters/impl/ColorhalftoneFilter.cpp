#include "ColorhalftoneFilter.h"

#include "gpu_engine/config.h"


#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <gpu_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/filters/BasicFilter.h"
#include "gpu_engine/gpu/VkGPUHelper.h"
#include "gpu_engine/gpu/compute_graph/BufferCopyNode.h"
#include "gpu_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "gpu_engine/log/Log.h"

VkResult ColorhalftoneFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                    const std::vector<FilterImageInfo> &inputImageInfo,
                                    const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->colorhalftoneFilterParams.imageSize.width = inputImageInfo[0].width;
    this->colorhalftoneFilterParams.imageSize.height = inputImageInfo[0].height;
    this->colorhalftoneFilterParams.imageSize.channels = 4;
    this->colorhalftoneFilterParams.imageSize.bytesPerLine = this->colorhalftoneFilterParams.imageSize.width * 4;

    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    VkResult ret = this->computeSubGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(ColorhalftoneFilterParams);
    pushConstantInfo.data = &this->colorhalftoneFilterParams;

    lookupBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    lookupBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, size * sizeof(float));
    lookupBuffer->UploadData(lookup, size * sizeof(float));

    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buffer = inputImageInfo[0].storageBuffer;
    pipelineNodeInput.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer pipelineNodelookupInput;
    pipelineNodelookupInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodelookupInput.buffer = lookupBuffer->GetBuffer();
    pipelineNodelookupInput.bufferSize = size * sizeof(float);

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buffer = outputImageInfo[0].storageBuffer;
    pipelineNodeOutput.bufferSize = outputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(pipelineNodeInput);
    vPipelineBuffers.push_back(pipelineNodelookupInput);
    vPipelineBuffers.push_back(pipelineNodeOutput);

    const auto kCalculateNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                      "colorhalftone",
                                                                      SHADER(colorhalftone.comp.glsl.spv),
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

void ColorhalftoneFilter::Destroy() {
    if (computeGraph != nullptr) {
        computeSubGraph->Destroy();
        computeGraph = nullptr;
    }
    if (lookupBuffer != nullptr) {
        lookupBuffer->Destroy();
        lookupBuffer = nullptr;
    }
}
