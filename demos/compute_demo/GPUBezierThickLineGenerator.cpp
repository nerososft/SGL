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
    if (bezierNode != nullptr) {
        bezierNode->Destroy();
        bezierNode = nullptr;
    }
    if (computeSubGraph != nullptr) {
        computeSubGraph->Destroy();
        computeSubGraph = nullptr;
    }
    if (computeGraph != nullptr) {
        computeGraph->Destroy();
        computeGraph = nullptr;
    }
}

bool GPUBezierThickLineGenerator::InitializeGPUPipeline() {
    std::vector<const char *> extensions = {};
    gpuCtx = std::make_shared<VkGPUContext>(extensions);

    VkResult result = VK_SUCCESS;

    result = gpuCtx->Init();
    if (result != VK_SUCCESS) {
        Logger() << "Failed to initialize GPU context!" << std::endl;
        return false;
    }

    computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    result = computeSubGraph->Init();
    if (result != VK_SUCCESS) {
        Logger() << "Failed to initialize sub graph!" << std::endl;
        return false;
    }
    computeGraph->AddSubGraph(computeSubGraph);
    bezierNode = std::make_shared<ComputePipelineNode>(gpuCtx, "BezierThickLine",
                                                       SHADER(bezier_thick.comp.glsl.spv),
                                                       (params.numPoints + 255) / 256,
                                                       1,
                                                       1);

    inputBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    result = inputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                          MAX_LINE_NUMS * sizeof(BezierLine));
    if (result != VK_SUCCESS) {
        Logger() << "Failed to allocate GPU buffer!" << std::endl;
        return false;
    }

    outputBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    result = outputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                           MAX_LINE_NUMS * params.numPoints * sizeof(Point2D) * 2);
    if (result != VK_SUCCESS) {
        Logger() << "Failed to allocate GPU buffer!" << std::endl;
        return false;
    }

    pixelMapBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    result = pixelMapBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                             params.numPoints * params.numPoints * 4);
    if (result != VK_SUCCESS) {
        Logger() << "Failed to allocate GPU buffer!" << std::endl;
        return false;
    }

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
    ppBuffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
        .buf = {
            .bufferSize = pixelMapBuffer->GetBufferSize(),
            .buffer = pixelMapBuffer->GetBuffer()
        }
    });

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
    result = bezierNode->CreateComputeGraphNode();
    if (result != VK_SUCCESS) {
        Logger() << "Failed to create compute graph node!" << std::endl;
        return false;
    }

    result = outputBuffer->MapBuffer();
    if (result != VK_SUCCESS) {
        Logger() << "Failed to map output buffer!" << std::endl;
        return false;
    }

    computeSubGraph->AddComputeGraphNode(bezierNode);
    return true;
}

Point2D *GPUBezierThickLineGenerator::GenerateThickLine(const std::vector<BezierLine> &lines) const {
    inputBuffer->UploadData(lines.data(), lines.size() * sizeof(BezierLine));

    const uint64_t time = TimeUtils::GetCurrentMonoMs();
    if (const VkResult ret = computeGraph->Compute(); ret != VK_SUCCESS) {
        Logger() << "Failed to compute graph!" << std::endl;
        return nullptr;
    }
    const uint64_t elapsed = TimeUtils::GetCurrentMonoMs() - time;
    Logger() << "TimeUsage: " << elapsed << "ms" << std::endl;

    return static_cast<Point2D *>(outputBuffer->GetMappedAddr());
}

void GPUBezierThickLineGenerator::GeneratePixelMap(const std::string &path) const {
    if (params.debugPixelMap) {
        if (const VkResult ret = pixelMapBuffer->MapBuffer(); ret != VK_SUCCESS) {
            Logger() << "Failed to map pixel buffer!" << std::endl;
            return;
        }
        ImageUtils::WritePngFile(path,
                                 params.numPoints,
                                 params.numPoints,
                                 4,
                                 pixelMapBuffer->GetMappedAddr());
        pixelMapBuffer->UnMapBuffer();
    }
}
