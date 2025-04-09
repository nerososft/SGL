#include "PathBlurFilter.h"

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

VkResult pathBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                               const std::vector<FilterImageInfo> &inputImageInfo,
                               const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->pathblurFilterParams.imageSize.width = inputImageInfo[0].width;
    this->pathblurFilterParams.imageSize.height = inputImageInfo[0].height;
    this->pathblurFilterParams.imageSize.channels = 4;
    this->pathblurFilterParams.imageSize.bytesPerLine = this->pathblurFilterParams.imageSize.width * 4;

    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    VkResult ret = this->computeSubGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(pathBlurFilterParams);
    pushConstantInfo.data = &this->pathblurFilterParams;

    vecBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    vecBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, k_size * sizeof(float));
    vecBuffer->UploadData(vec, k_size * sizeof(float));

    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buffer = inputImageInfo[0].storageBuffer;
    pipelineNodeInput.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer pipelineNodeKInput;
    pipelineNodeKInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeKInput.buffer = vecBuffer->GetBuffer();
    pipelineNodeKInput.bufferSize = k_size * sizeof(float);

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buffer = outputImageInfo[0].storageBuffer;
    pipelineNodeOutput.bufferSize = outputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(pipelineNodeInput);
    vPipelineBuffers.push_back(pipelineNodeKInput);
    vPipelineBuffers.push_back(pipelineNodeOutput);

    const auto kCalculateNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                      "pathblur",
                                                                      SHADER(pathblur.comp.glsl.spv),
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

void pathBlurFilter::Destroy() {
    computeGraph->Destroy();
    vecBuffer->Destroy();
}
