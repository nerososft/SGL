//
// Created by neo on 2025/3/13.
//

#include "ScaleFilter.h"

#include "gpu_engine/config.h"

#include "effect_engine/gpu/compute_graph/BufferCopyNode.h"
#include "effect_engine/gpu/compute_graph/ComputePipelineNode.h"
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/log/Log.h"

VkResult ScaleFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                            const std::vector<FilterImageInfo> &inputImageInfo,
                          const std::vector<FilterImageInfo> &outputImageInfo) {




    BasicFilterParams params;
    this->scaleFilterParams.imageSize.width = inputImageInfo[0].width;
    this->scaleFilterParams.imageSize.height = inputImageInfo[0].height;
    this->scaleFilterParams.imageSize.channels = 4;
    this->scaleFilterParams.imageSize.bytesPerLine = this->scaleFilterParams.imageSize.width * 4;


    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);

    VkResult ret = this->computeSubGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(ScaleFilterParams);
    pushConstantInfo.data = &this->scaleFilterParams;



    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buffer = inputImageInfo[0].storageBuffer;
    pipelineNodeInput.bufferSize = inputImageInfo[0].bufferSize;



    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buffer = outputImageInfo[0].storageBuffer;
    pipelineNodeOutput.bufferSize = outputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(pipelineNodeInput);
    vPipelineBuffers.push_back(pipelineNodeOutput);

    const auto kCalculateNode = std::make_shared<ComputePipelineNode>(gpuCtx,
        "scaleImage",
        SHADER(scale.comp.glsl.spv),
        (outputImageInfo[0].width + 31) / 32,
        (outputImageInfo[0].height + 31) / 32,
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

void ScaleFilter::Destroy() {
    BasicFilter::Destroy();
}
