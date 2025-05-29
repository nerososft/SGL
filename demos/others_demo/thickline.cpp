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

#include "gpu_engine/utils/ImageUtils.h"
#include "gpu_engine/utils/TimeUtils.h"

struct Point2D {
    float x;
    float y;
};

struct BezierParams {
    uint lineNums;

    uint numPoints; // 生成的点数量
    float tStart; // 参数t的起始值
    float tEnd; // 参数t的结束值

    uint width;
    uint height;
} bezierParams = {};

struct BezierLine {
    Point2D points[4];
    float beginWidth;
    float endWidth;
};

std::vector<std::vector<Point2D> > thickline(std::vector<BezierLine> lines) {
}

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
    outputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, bezierParams.numPoints * sizeof(Point2D));

    static const auto pixelBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    pixelBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, bezierParams.numPoints * bezierParams.numPoints * 4);

    const auto bezierNode = std::make_shared<ComputePipelineNode>(gpuCtx, "BezierThickLine",
                                                                  SHADER(bezier_thick.comp.glsl.spv),
                                                                  (bezierParams.numPoints + 255) / 256,
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

    ppBuffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
        .buf = {
            .bufferSize = pixelBuffer->GetBufferSize(),
            .buffer = pixelBuffer->GetBuffer()
        }
    });

    constexpr PushConstantInfo pushConstantInfo{
        .size = sizeof(BezierParams),
        .data = &bezierParams,
    };
    const ComputeElement element = {
        .pushConstantInfo = pushConstantInfo,
        .buffers = ppBuffers,
        .customDrawFunc = nullptr
    };

    bezierNode->AddComputeElement(element);

    if (bezierNode->CreateComputeGraphNode() != VK_SUCCESS) {
        std::cerr << "Failed to create compute graph node!" << std::endl;
        return nullptr;
    }

    computeSubGraph->AddComputeGraphNode(bezierNode);

    const uint64_t time = TimeUtils::GetCurrentMonoMs();
    VkResult ret = computeGraph->Compute();
    if (ret != VK_SUCCESS) {
        std::cerr << "Failed to compute graph!" << std::endl;
        return nullptr;
    }
    const uint64_t elapsed = TimeUtils::GetCurrentMonoMs() - time;
    std::cout << "TimeUsage: " << elapsed << "ms" << std::endl;

    ret = outputBuffer->MapBuffer();
    if (ret != VK_SUCCESS) {
        std::cerr << "Failed to map buffer!" << std::endl;
        return nullptr;
    }

    ret = pixelBuffer->MapBuffer();
    if (ret != VK_SUCCESS) {
        std::cerr << "Failed to map pixel buffer!" << std::endl;
        return nullptr;
    }
    ImageUtils::WritePngFile("../../../demos/others_demo/line.png", bezierParams.numPoints, bezierParams.numPoints, 4,
                             pixelBuffer->GetMappedAddr());

    return static_cast<Point2D *>(outputBuffer->GetMappedAddr());
}

int main(int argc, char *argv[]) {
    std::cout << "mindmaster_demo" << std::endl;

    std::vector<Point2D> points{};
    for (uint32_t i = 0; i < 100; i++) {
        points.push_back({.x = 0.0, .y = 100.0f + 8.0f * static_cast<float>(i)});
        points.push_back({.x = 600.0, .y = -100.0});
        points.push_back({.x = 400.0, .y = 1100.0});
        points.push_back({.x = 1000.0, .y = 900.0});
    }
    bezierParams.lineNums = 100;

    bezierParams.numPoints = 1000;
    bezierParams.tStart = 0.0f;
    bezierParams.tEnd = 1.0f;

    bezierParams.width = 1000;
    bezierParams.height = 1000;
    const Point2D *result = thickLine(points);
    if (result != nullptr) {
        for (const auto [x, y]: points) {
            std::cout << "[" << x << "," << y << "]";
        }
        std::cout << std::endl;
        for (int i = 0; i < bezierParams.numPoints; i++) {
            std::cout << "[" << result[i].x << "," << result[i].y << "]";
        }
        std::cout << std::endl;
    }
    return 0;
}
