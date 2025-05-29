//
// Created by neo on 25-5-20.
//

#include <iostream>
#include <memory>
#include <ostream>
#include <vector>
#include <gpu_engine/config.h>
#include <gpu_engine/gpu/VkGPUBuffer.h>
#include <gpu_engine/gpu/VkGPUContext.h>
#include <gpu_engine/gpu/compute_graph/ComputeGraph.h>
#include <gpu_engine/gpu/compute_graph/ComputePipelineNode.h>

struct Point2D {
    float x;
    float y;
};

struct Params {
    float thickness;
    float endThickness;
    float dz;
} params = {};

Point2D *thickLine(const std::vector<Point2D> &points) {
    std::vector<const char *> extensions = {};
    auto gpuCtx = std::make_shared<VkGPUContext>(extensions);

    if (gpuCtx->Init() != VK_SUCCESS) {
        std::cerr << "Failed to initialize GPU context!" << std::endl;
        return nullptr;
    }

    const auto computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    const auto computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    if (computeSubGraph->Init() != VK_SUCCESS) {
        std::cerr << "Failed to initialize sub graph!" << std::endl;
        return nullptr;
    }
    computeGraph->AddSubGraph(computeSubGraph);

    const VkDeviceSize pointsSize = points.size() * sizeof(Point2D);
    const auto inputBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    inputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, pointsSize);
    inputBuffer->UploadData(points.data(), pointsSize);

    static const auto outputBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    outputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, pointsSize * 2);

    const auto transformNode = std::make_shared<ComputePipelineNode>(gpuCtx, "thickline",
                                                                     SHADER(thickline.comp.glsl.spv),
                                                                     (points.size() + 255) / 256,
                                                                     1,
                                                                     1);

    std::vector<PipelineNodeBuffer> ppBuffers;
    PipelineNodeBuffer input;
    ppBuffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_READ,
        .buf = {
            .bufferSize = inputBuffer->GetBufferSize(),
            .buffer = inputBuffer->GetBuffer()
        }
    });
    ppBuffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
        .buf = {
            .bufferSize = outputBuffer->GetBufferSize(),
            .buffer = outputBuffer->GetBuffer()
        }
    });

    constexpr PushConstantInfo pushConstantInfo{
        .size = sizeof(Params),
        .data = &params,
    };
    const ComputeElement element = {
        .pushConstantInfo = pushConstantInfo,
        .buffers = ppBuffers,
        .customDrawFunc = nullptr
    };

    transformNode->AddComputeElement(element);

    if (transformNode->CreateComputeGraphNode() != VK_SUCCESS) {
        std::cerr << "Failed to create compute graph node!" << std::endl;
        return nullptr;
    }

    computeSubGraph->AddComputeGraphNode(transformNode);

    VkResult ret = computeGraph->Compute();
    if (ret != VK_SUCCESS) {
        std::cerr << "Failed to compute graph!" << std::endl;
        return nullptr;
    }

    ret = outputBuffer->MapBuffer();
    if (ret != VK_SUCCESS) {
        std::cerr << "Failed to map buffer!" << std::endl;
        return nullptr;
    }

    return static_cast<Point2D *>(outputBuffer->GetMappedAddr());
}

int main(int argc, char *argv[]) {
    std::cout << "mindmaster_demo" << std::endl;

    const std::vector<Point2D> points{
        {.x = 10.0f, .y = 12.0f,},
        {.x = 11.0f, .y = 12.0f,},
        {.x = 12.0f, .y = 12.0f,},
        {.x = 13.0f, .y = 13.0f,},
    };
    params.thickness = 5.0f;
    params.endThickness = 0.0f;
    params.dz = 0.1f;
    const Point2D *result = thickLine(points);
    if (result != nullptr) {
        for (const auto [x, y]: points) {
            std::cout << "[" << x << "," << y << "]";
        }
        std::cout << std::endl;
        for (int i = 0; i < points.size() * 2; i++) {
            std::cout << "[" << result[i].x << "," << result[i].y << "]";
        }
        std::cout << std::endl;
    }
    return 0;
}
