//
// Created by 1234 on 2025/3/6.
//

#include "customKernelFilter.h"

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


VkResult customKernelFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           const VkDeviceSize bufferSize,
                           const uint32_t width,
                           const uint32_t height,
                           const VkBuffer inputBuffer,
                           const VkBuffer outputBuffer) {
    BasicFilterParams params;
    this->kFilterParams.imageSize.width = width;
    this->kFilterParams.imageSize.height = height;
    this->kFilterParams.imageSize.channels = 4;
    this->kFilterParams.imageSize.bytesPerLine = this->kFilterParams.imageSize.width * 4;
    //params.paramsSize = sizeof(customKernelFilterParams);
    //params.paramsData = &this->kFilterParams;
    //params.shaderPath = SHADER(customKernel.comp.glsl.spv);

    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    VkResult ret = this->computeGraph->Init();
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

 //   int k_size = 25 * sizeof(int);
    kBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    kBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, k_size * sizeof(int) );
    //kBuffer->GetBuffer();
    kBuffer->UploadData(k , k_size * sizeof(int) );

    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buffer = inputBuffer;
    pipelineNodeInput.bufferSize = bufferSize;

    PipelineNodeBuffer pipelineNodeKInput;
    pipelineNodeKInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeKInput.buffer = kBuffer->GetBuffer();
    pipelineNodeKInput.bufferSize = bufferSize;

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buffer = outputBuffer;
    pipelineNodeOutput.bufferSize = bufferSize;

    std::vector<PipelineNodeBuffer> vPipelineBuffers;
    vPipelineBuffers.push_back(pipelineNodeInput);
    vPipelineBuffers.push_back(pipelineNodeKInput);
    vPipelineBuffers.push_back(pipelineNodeOutput);

    const auto kCalculateNode = std::make_shared<ComputePipelineNode>(gpuCtx,
        "KCalculate",
        SHADER(custom_kernel.comp.glsl.spv),
        (width + 31) / 32,
        (height + 31) / 32,
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

    computeGraph->AddComputeGraphNode(kCalculateNode);

    return computeGraph->Compute();
}

void customKernelFilter::Destroy() {
    Logger() << "customKernelFilter begin" << std::endl;
    kBuffer->Destroy();
    BasicFilter::Destroy();


}
