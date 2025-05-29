//
// Created by neo on 25-5-20.
//

#include <iostream>
#include <memory>
#include <ostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <gpu_engine/config.h>
#include <gpu_engine/gpu/VkGPUBuffer.h>
#include <gpu_engine/gpu/VkGPUContext.h>
#include <gpu_engine/gpu/compute_graph/ComputeGraph.h>
#include <gpu_engine/gpu/compute_graph/ComputePipelineNode.h>

struct Point2D {
    float x;
    float y;
};

auto trans = glm::mat4(1);

Point2D *transform(const std::vector<Point2D> &points) {
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
    outputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, pointsSize);

    const auto transformNode = std::make_shared<ComputePipelineNode>(gpuCtx, "Transform",
                                                                     SHADER(demo_transform.comp.glsl.spv),
                                                                     (points.size() + 255) / 256,
                                                                     1,
                                                                     1);

    std::vector<PipelineNodeBuffer> ppBuffers;
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
        .size = sizeof(trans),
        .data = &trans,
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
        {.x = 10.0f, .y = 12.0f,}
    };
    trans = glm::translate(glm::mat4(1), glm::vec3(5, 8, 0));

    const Point2D *result = transform(points);
    if (result != nullptr) {
        for (int i = 0; i < points.size(); i++) {
            std::cout << "(" << points[i].x << ", " << points[i].y << ") -> (" << result[i].x << ", " << result[i].y <<
                    ")" << std::endl;
        }
    }
    return 0;
}
