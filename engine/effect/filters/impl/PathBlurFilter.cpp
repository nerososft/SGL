#include "PathBlurFilter.h"

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

    /*vecBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    vecBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, k_size * sizeof(float));
    vecBuffer->UploadData(vec, k_size * sizeof(float));*/
    //
    PipelineNodeBuffer vecpipelineNodeInput;
    vecpipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    vecpipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
    vecpipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

    int vecsize = 2 * int(this->pathblurFilterParams.num + 1);

    startposBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    startposBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, vecsize * sizeof(float));
    startposBuffer->UploadData(startpos, vecsize * sizeof(float));

    PipelineNodeBuffer vecpipelineNodeSpInput;
    vecpipelineNodeSpInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    vecpipelineNodeSpInput.buf.buffer = startposBuffer->GetBuffer();
    vecpipelineNodeSpInput.buf.bufferSize = vecsize * sizeof(float);

    endposBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    endposBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, vecsize * sizeof(float));
    endposBuffer->UploadData(endpos, vecsize * sizeof(float));

    PipelineNodeBuffer vecpipelineNodeEpInput;
    vecpipelineNodeEpInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    vecpipelineNodeEpInput.buf.buffer = endposBuffer->GetBuffer();
    vecpipelineNodeEpInput.buf.bufferSize = vecsize * sizeof(float);

    startvecBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    startvecBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, vecsize * sizeof(float));
    startvecBuffer->UploadData(startvec, vecsize * sizeof(float));

    PipelineNodeBuffer vecpipelineNodeSvInput;
    vecpipelineNodeSvInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    vecpipelineNodeSvInput.buf.buffer = startvecBuffer->GetBuffer();
    vecpipelineNodeSvInput.buf.bufferSize = vecsize * sizeof(float);

    endvecBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    endvecBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, vecsize * sizeof(float));
    endvecBuffer->UploadData(endvec, vecsize * sizeof(float));

    PipelineNodeBuffer vecpipelineNodeEvInput;
    vecpipelineNodeEvInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    vecpipelineNodeEvInput.buf.buffer = endvecBuffer->GetBuffer();
    vecpipelineNodeEvInput.buf.bufferSize = vecsize * sizeof(float);

    vecBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    vecBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_LOCAL, k_size * sizeof(float));

    PipelineNodeBuffer vecpipelineNodeOutput;
    vecpipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    vecpipelineNodeOutput.buf.buffer = vecBuffer->GetBuffer();
    vecpipelineNodeOutput.buf.bufferSize = k_size * sizeof(float);

    std::vector<PipelineNodeBuffer> vecPipelineBuffers;
    vecPipelineBuffers.push_back(vecpipelineNodeInput);
    vecPipelineBuffers.push_back(vecpipelineNodeSpInput);
    vecPipelineBuffers.push_back(vecpipelineNodeEpInput);
    vecPipelineBuffers.push_back(vecpipelineNodeSvInput);
    vecPipelineBuffers.push_back(vecpipelineNodeEvInput);
    vecPipelineBuffers.push_back(vecpipelineNodeOutput);

    const auto vecCalculateNode = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                                        "vecpathblurCalculate",
                                                                        SHADER(vecpathblur.comp.glsl.spv),
                                                                        (inputImageInfo[0].width + 31) / 32,
                                                                        (inputImageInfo[0].height + 31) / 32,
                                                                        1);

    vecCalculateNode->AddComputeElement({
        .pushConstantInfo = pushConstantInfo,
        .buffers = vecPipelineBuffers
    });

    ret = vecCalculateNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }
    //
    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
    pipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

    PipelineNodeBuffer pipelineNodeKInput;
    pipelineNodeKInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeKInput.buf.buffer = vecBuffer->GetBuffer();
    pipelineNodeKInput.buf.bufferSize = k_size * sizeof(float);

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buf.buffer = outputImageInfo[0].storageBuffer;
    pipelineNodeOutput.buf.bufferSize = outputImageInfo[0].bufferSize;

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

    kCalculateNode->AddDependenceNode(vecCalculateNode);
    computeSubGraph->AddComputeGraphNode(kCalculateNode);
    computeGraph->AddSubGraph(computeSubGraph);

    return computeGraph->Compute();
}

void pathBlurFilter::Destroy() {
    if (computeGraph != nullptr) {
        computeGraph->Destroy();
        computeGraph = nullptr;
    }
    if (vecBuffer != nullptr) {
        vecBuffer->Destroy();
        vecBuffer = nullptr;
    }
    if (startposBuffer != nullptr) {
        startposBuffer->Destroy();
        startposBuffer = nullptr;
    }
    if (endposBuffer != nullptr) {
        endposBuffer->Destroy();
        endposBuffer = nullptr;
    }
    if (startvecBuffer != nullptr) {
        startvecBuffer->Destroy();
        startvecBuffer = nullptr;
    }
    if (endvecBuffer != nullptr) {
        endvecBuffer->Destroy();
        endvecBuffer = nullptr;
    }
}
