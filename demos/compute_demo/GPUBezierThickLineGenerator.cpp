//
// Created by neo on 25-6-3.
//

#include "GPUBezierThickLineGenerator.h"

#include "gpu_engine/gpu/compute_graph/ComputeGraph.h"
#include "gpu_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "gpu_engine/log/Log.h"
#include "gpu_engine/utils/ImageUtils.h"
#include "gpu_engine/utils/TimeUtils.h"

GPUBezierThickLineGenerator::~GPUBezierThickLineGenerator() {
    if (this->pixelMapBuffer != nullptr) {
        this->pixelMapBuffer->Destroy();
        this->pixelMapBuffer = nullptr;
    }
    if (this->outputBuffer != nullptr) {
        this->outputBuffer->UnMapBuffer();
        this->outputBuffer->Destroy();
        this->outputBuffer = nullptr;
    }
    if (this->inputBuffer != nullptr) {
        this->inputBuffer->Destroy();
        this->inputBuffer = nullptr;
    }
}

bool GPUBezierThickLineGenerator::InitializeGPUPipeline() {
    std::vector<const char *> extensions = {};
    gpuCtx = std::make_shared<VkGPUContext>(extensions);

    if (gpuCtx->Init() != VK_SUCCESS) {
        Logger() << "Failed to initialize GPU context!" << std::endl;
        return false;
    }

    computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    if (computeSubGraph->Init() != VK_SUCCESS) {
        Logger() << "Failed to initialize sub graph!" << std::endl;
        return false;
    }
    computeGraph->AddSubGraph(computeSubGraph);


    bezierNode = std::make_shared<ComputePipelineNode>(gpuCtx, "BezierThickLine",
                                                       SHADER(bezier_thick.comp.glsl.spv),
                                                       (params.numPoints + 255) / 256,
                                                       1,
                                                       1);
    return true;
}

Point2D *GPUBezierThickLineGenerator::GenerateThickLine(const std::vector<BezierLine> &lines) {
    const VkDeviceSize pointsSize = lines.size() * sizeof(BezierLine);
    inputBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    inputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, pointsSize);
    inputBuffer->UploadData(lines.data(), pointsSize);

    outputBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    outputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                  lines.size() * params.numPoints * sizeof(Point2D) * 2);

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

    if (params.debugPixelMap) {
        pixelMapBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
        pixelMapBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, params.numPoints * params.numPoints * 4);
        ppBuffers.push_back({
            .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
            .buf = {
                .bufferSize = pixelMapBuffer->GetBufferSize(),
                .buffer = pixelMapBuffer->GetBuffer()
            }
        });
    }

    const PushConstantInfo pushConstantInfo{
        .size = sizeof(BezierParams),
        .data = &params,
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

    if (params.debugPixelMap) {
        ret = pixelMapBuffer->MapBuffer();
        if (ret != VK_SUCCESS) {
            std::cerr << "Failed to map pixel buffer!" << std::endl;
            return nullptr;
        }
        ImageUtils::WritePngFile("../../../demos/compute_demo/line.png",
                                 params.numPoints,
                                 params.numPoints,
                                 4,
                                 pixelMapBuffer->GetMappedAddr());
        pixelMapBuffer->UnMapBuffer();
    }

    ret = outputBuffer->MapBuffer();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to map output buffer!" << std::endl;
        return nullptr;
    }

    return static_cast<Point2D *>(outputBuffer->GetMappedAddr());
}
