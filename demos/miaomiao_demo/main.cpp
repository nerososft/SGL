//
// Created by neo on 25-5-9.
//

#include <iostream>
#include <ostream>

#include "effect_engine/EffectEngine.h"
#include "gpu_engine/config.h"
#include "gpu_engine/gpu/VkGPUBuffer.h"
#include "gpu_engine/gpu/VkGPUContext.h"
#include "gpu_engine/gpu/compute_graph/ComputeGraph.h"
#include "gpu_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "gpu_engine/utils/TimeUtils.h"


void cpu_gpu_mem_share_demo(const std::shared_ptr<VkGPUContext> &gpuCtx) {
    const VkDeviceSize bufferSize = 1024 * 1024 * 4;

    const auto sharedMem = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (sharedMem->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, bufferSize) != VK_SUCCESS) {
        std::cerr << "Failed to allocate shared memory!" << std::endl;
        return;
    };
    if (sharedMem->MapBuffer(bufferSize) != VK_SUCCESS) {
        std::cerr << "Failed to map shared memory buffer!" << std::endl;
        return;
    }

    void *sharedMemAddr = sharedMem->GetMappedAddr();
    static_cast<uint32_t *>(sharedMemAddr)[0] = 0xFFFFFFFF; // ACCESS from CPU

    sharedMem->UnMapBuffer();

    VkBuffer buffer = sharedMem->GetBuffer(); // this can be accessed in GPU
}

void blender_demo(const std::shared_ptr<VkGPUContext> &gpuCtx) {
    const auto computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    const auto computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    if (computeSubGraph->Init() != VK_SUCCESS) {
        std::cerr << "Failed to initialize sub graph!" << std::endl;
        return;
    }
    computeGraph->AddSubGraph(computeSubGraph);

    const uint32_t width = 1920;
    const uint32_t height = 1080;
    const VkDeviceSize bufferSize = width * height * 4;

    const auto inputBuffer1 = std::make_shared<VkGPUBuffer>(gpuCtx);
    inputBuffer1->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, bufferSize);

    const auto inputBuffer2 = std::make_shared<VkGPUBuffer>(gpuCtx);
    inputBuffer2->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, bufferSize);

    const auto inputBuffer3 = std::make_shared<VkGPUBuffer>(gpuCtx);
    inputBuffer3->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, bufferSize);

    const auto inputBuffer4 = std::make_shared<VkGPUBuffer>(gpuCtx);
    inputBuffer4->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, bufferSize);

    const auto inputBuffer5 = std::make_shared<VkGPUBuffer>(gpuCtx);
    inputBuffer5->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, bufferSize);

    const auto outputBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    outputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, bufferSize);

    const auto blendNode = std::make_shared<ComputePipelineNode>(gpuCtx, "blend",
                                                                 SHADER(blend5.comp.glsl.spv),
                                                                 32, 32,
                                                                 0);

    std::vector<PipelineNodeBuffer> ppBuffers;
    ppBuffers.push_back({
        .buffer = inputBuffer1->GetBuffer(),
        .type = PIPELINE_NODE_BUFFER_STORAGE_READ,
        .bufferSize = inputBuffer1->GetBufferSize()
    });
    ppBuffers.push_back({
        .buffer = inputBuffer2->GetBuffer(),
        .type = PIPELINE_NODE_BUFFER_STORAGE_READ,
        .bufferSize = inputBuffer2->GetBufferSize()
    });
    ppBuffers.push_back({
        .buffer = inputBuffer3->GetBuffer(),
        .type = PIPELINE_NODE_BUFFER_STORAGE_READ,
        .bufferSize = inputBuffer3->GetBufferSize()
    });
    ppBuffers.push_back({
        .buffer = inputBuffer4->GetBuffer(),
        .type = PIPELINE_NODE_BUFFER_STORAGE_READ,
        .bufferSize = inputBuffer4->GetBufferSize()
    });
    ppBuffers.push_back({
        .buffer = inputBuffer5->GetBuffer(),
        .type = PIPELINE_NODE_BUFFER_STORAGE_READ,
        .bufferSize = inputBuffer5->GetBufferSize()
    });
    ppBuffers.push_back({
        .buffer = outputBuffer->GetBuffer(),
        .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
        .bufferSize = outputBuffer->GetBufferSize()
    });

    const PushConstantInfo pushConstantInfo{};
    const ComputeElement element = {
        .customDrawFunc = nullptr,
        .pushConstantInfo = pushConstantInfo,
        .buffers = ppBuffers,
    };

    blendNode->AddComputeElement(element);

    if (blendNode->CreateComputeGraphNode() != VK_SUCCESS) {
        std::cerr << "Failed to create compute graph node!" << std::endl;
        return;
    }

    computeSubGraph->AddComputeGraphNode(blendNode);

    const uint64_t blendStart = TimeUtils::GetCurrentMonoMs();
    if (computeGraph->Compute() != VK_SUCCESS) {
        std::cerr << "Failed to compute graph!" << std::endl;
        return;
    }
    const uint64_t blendEnd = TimeUtils::GetCurrentMonoMs();
    std::cout << "Blend Usage=" << blendEnd - blendStart << "ms" << std::endl;
}

int main(int argc, char *argv[]) {
    std::cout << "MiaoYing demo" << std::endl;

    std::vector<const char *> extensions = {};
    auto gpuCtx = std::make_shared<VkGPUContext>(extensions);

    if (gpuCtx->Init() != VK_SUCCESS) {
        std::cerr << "Failed to initialize GPU context!" << std::endl;
        return 0;
    }

    cpu_gpu_mem_share_demo(gpuCtx);

    blender_demo(gpuCtx);
    return 0;
}
