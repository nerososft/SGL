#include "TiltshiftBlurFilter.h"

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

VkResult TiltshiftBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                    const std::vector<FilterImageInfo> &inputImageInfo,
                                    const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->tiltshiftblurFilterParams.imageSize.width = inputImageInfo[0].width;
    this->tiltshiftblurFilterParams.imageSize.height = inputImageInfo[0].height;
    this->tiltshiftblurFilterParams.imageSize.channels = 4;
    this->tiltshiftblurFilterParams.imageSize.bytesPerLine = this->tiltshiftblurFilterParams.imageSize.width * 4;

    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    VkResult ret = this->computeSubGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(TiltshiftBlurFilterParams);
    pushConstantInfo.data = &this->tiltshiftblurFilterParams;

    ABuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    ABuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, size * sizeof(float));
    ABuffer->UploadData(A, size * sizeof(float));

    BBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    BBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, size * sizeof(float));
    BBuffer->UploadData(B, size * sizeof(float));

    CBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    CBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, size * sizeof(float));
    CBuffer->UploadData(C, size * sizeof(float));

    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
    pipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer pipelineNodeTempInput;
    pipelineNodeTempInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeTempInput.buf.buffer = inputImageInfo[1].storageBuffer;
    pipelineNodeTempInput.buf.bufferSize = inputImageInfo[1].bufferSize;

    PipelineNodeBuffer pipelineNodeAInput;
    pipelineNodeAInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeAInput.buf.buffer = ABuffer->GetBuffer();
    pipelineNodeAInput.buf.bufferSize = size * sizeof(float);

    PipelineNodeBuffer pipelineNodeBInput;
    pipelineNodeBInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeBInput.buf.buffer = BBuffer->GetBuffer();
    pipelineNodeBInput.buf.bufferSize = size * sizeof(float);

    PipelineNodeBuffer pipelineNodeCInput;
    pipelineNodeCInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeCInput.buf.buffer = CBuffer->GetBuffer();
    pipelineNodeCInput.buf.bufferSize = size * sizeof(float);

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buf.buffer = outputImageInfo[0].storageBuffer;
    pipelineNodeOutput.buf.bufferSize = outputImageInfo[0].bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(pipelineNodeInput);
    vPipelineBuffers.push_back(pipelineNodeTempInput);
    vPipelineBuffers.push_back(pipelineNodeAInput);
    vPipelineBuffers.push_back(pipelineNodeBInput);
    vPipelineBuffers.push_back(pipelineNodeCInput);
    vPipelineBuffers.push_back(pipelineNodeOutput);

    const auto kCalculateNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                      "tiltshiftblur",
                                                                      SHADER(tiltshiftblur.comp.glsl.spv),
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

void TiltshiftBlurFilter::Destroy() {
    if (computeGraph != nullptr) {
        computeSubGraph->Destroy();
        computeGraph = nullptr;
    }
    if (ABuffer != nullptr) {
        ABuffer->Destroy();
        ABuffer = nullptr;
    }
    if (BBuffer != nullptr) {
        BBuffer->Destroy();
        BBuffer = nullptr;
    }
    if (CBuffer != nullptr) {
        CBuffer->Destroy();
        CBuffer = nullptr;
    }
}
