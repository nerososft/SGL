//
// Created by neo on 25-4-3.
//

#include "MedianFilter.h"

#include <memory>
#include <effect_engine/config.h>
#include <effect_engine/filters/BasicFilter.h>
#include <effect_engine/gpu/VkGPUContext.h>
#include <vulkan/vulkan_core.h>

#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "effect_engine/log/Log.h"

std::shared_ptr<SubComputeGraph> MedianFilter::CreateParallelSubGraph(const size_t parallelIndex,
                                                                      const std::shared_ptr<VkGPUContext> &gpuCtx,
                                                                      const VkBuffer inputBuffer,
                                                                      const VkDeviceSize inputBufferSize,
                                                                      const VkBuffer outputBuffer,
                                                                      const VkDeviceSize outputBufferSize) {
    const auto computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    VkResult ret = computeSubGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return nullptr;
    }

    PushConstantInfo pushConstantInfo;
    pushConstantInfo.size = sizeof(MedianFilterParams);
    pushConstantInfo.data = &this->medianFilterParams[parallelIndex];

    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buffer = inputBuffer;
    pipelineNodeInput.bufferSize = inputBufferSize;

    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buffer = outputBuffer;
    pipelineNodeOutput.bufferSize = outputBufferSize;

    std::vector<PipelineNodeBuffer> pipelineBuffers;
    pipelineBuffers.push_back(pipelineNodeInput);
    pipelineBuffers.push_back(pipelineNodeOutput);

    const auto node = std::make_shared<ComputePipelineNode>(gpuCtx,
                                                            "MedianFilter",
                                                            SHADER(midvalue.comp.glsl.spv),
                                                            (this->medianFilterParams[parallelIndex].imageSize.width +
                                                             31) / 32,
                                                            (this->medianFilterParams[parallelIndex].imageSize.height +
                                                             31) / 32,
                                                            1);
    node->AddComputeElement({
        .pushConstantInfo = pushConstantInfo,
        .buffers = pipelineBuffers
    });
    ret = node->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return nullptr;
    }
    computeSubGraph->AddComputeGraphNode(node);
    return computeSubGraph;
}

VkResult MedianFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                             const VkDeviceSize bufferSize,
                             const uint32_t width,
                             const uint32_t height,
                             const VkBuffer inputBuffer,
                             const VkBuffer outputBuffer) {
    uint32_t parallelSize;
    const std::vector<DeviceQueue> parallelQueues = gpuCtx->GetAllParallelQueue();
    if (parallelQueues.size() >= 4) {
        parallelSize = 4;
    } else {
        parallelSize = parallelQueues.size();
    }
    Logger() << "Parallel size:" << parallelSize << ", all queue: " << parallelQueues.size() << std::endl;
    this->medianFilterParams.resize(parallelSize);
    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    for (size_t parallelIndex = 0; parallelIndex < parallelSize; parallelIndex++) {
        this->medianFilterParams[parallelIndex].imageSize.width = width;
        this->medianFilterParams[parallelIndex].imageSize.height = height;
        this->medianFilterParams[parallelIndex].imageSize.channels = 4;
        this->medianFilterParams[parallelIndex].imageSize.bytesPerLine =
                this->medianFilterParams[parallelIndex].imageSize.width * 4;
        this->medianFilterParams[parallelIndex].pieceCount = parallelSize;
        this->medianFilterParams[parallelIndex].piece = parallelIndex;
        this->medianFilterParams[parallelIndex].radius = radius;

        const std::shared_ptr<SubComputeGraph> parallelGraph = CreateParallelSubGraph(parallelIndex,
            gpuCtx,
            inputBuffer,
            bufferSize,
            outputBuffer,
            bufferSize);
        if (parallelGraph == nullptr) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }
        computeGraph->AddSubGraph(parallelGraph);
    }

    return computeGraph->Compute();
}

void MedianFilter::Destroy() {
    this->computeGraph->Destroy();
}
