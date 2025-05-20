//
// Created by neo on 25-5-20.
//

#include <iostream>
#include <memory>
#include <ostream>
#include <vector>
#include <glm/vec4.hpp>
#include <gpu_engine/config.h>
#include <gpu_engine/gpu/VkGPUBuffer.h>
#include <gpu_engine/gpu/VkGPUContext.h>
#include <gpu_engine/gpu/compute_graph/ComputeGraph.h>
#include <gpu_engine/gpu/compute_graph/ComputePipelineNode.h>

bool bezier_curve() {
    std::vector<const char *> extensions = {};
    auto gpuCtx = std::make_shared<VkGPUContext>(extensions);

    if (gpuCtx->Init() != VK_SUCCESS) {
        std::cerr << "Failed to initialize GPU context!" << std::endl;
        return false;
    }

    const auto computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    const auto computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    if (computeSubGraph->Init() != VK_SUCCESS) {
        std::cerr << "Failed to initialize sub graph!" << std::endl;
        return false;
    }
    computeGraph->AddSubGraph(computeSubGraph);

    const VkDeviceSize controlDataSize = 1;// TODO:
    const auto inputBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    inputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, controlDataSize);

    // BezierCurveControlData controlData;
    // inputBuffer->UploadData(&controlData, controlDataSize);

    VkDeviceSize outputPointDataSize = 1; // TODO:
    const auto outputBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    outputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, outputPointDataSize);

    const auto bezierNode = std::make_shared<ComputePipelineNode>(gpuCtx, "bezier_curve",
                                                                  SHADER(bezier_curve.comp.glsl.spv),
                                                                  256,
                                                                  1,
                                                                  1);

    std::vector<PipelineNodeBuffer> ppBuffers;
    ppBuffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_READ,
        .bufferSize = inputBuffer->GetBufferSize(),
        .buffer = inputBuffer->GetBuffer()
    });
    ppBuffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
        .bufferSize = outputBuffer->GetBufferSize(),
        .buffer = outputBuffer->GetBuffer()
    });

    const PushConstantInfo pushConstantInfo{};
    const ComputeElement element = {
        .pushConstantInfo = pushConstantInfo,
        .buffers = ppBuffers,
        .customDrawFunc = nullptr
    };

    bezierNode->AddComputeElement(element);

    if (bezierNode->CreateComputeGraphNode() != VK_SUCCESS) {
        std::cerr << "Failed to create compute graph node!" << std::endl;
        return false;
    }

    computeSubGraph->AddComputeGraphNode(bezierNode);


    const VkResult ret = computeGraph->Compute();
    if (ret != VK_SUCCESS) {
        std::cerr << "Failed to compute graph!" << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    std::cout << "mindmaster_demo" << std::endl;

    bezier_curve();

    return 0;
}
