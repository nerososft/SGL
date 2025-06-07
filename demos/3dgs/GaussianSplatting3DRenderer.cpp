//
// Created by neo on 25-6-3.
//

#include "GaussianSplatting3DRenderer.h"

#include "gpu_engine/gpu/compute_graph/ComputeGraph.h"
#include "gpu_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "gpu_engine/log/Log.h"
#include "gpu_engine/utils/ImageUtils.h"
#include "gpu_engine/utils/TimeUtils.h"

GaussianSplatting3DRenderer::~GaussianSplatting3DRenderer() {
    if (this->pixelMapBuffer != nullptr) {
        this->pixelMapBuffer->Destroy();
        this->pixelMapBuffer = nullptr;
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

bool GaussianSplatting3DRenderer::InitializeGPUPipeline(size_t maxPoints) {
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
    bezierNode = std::make_shared<ComputePipelineNode>(gpuCtx, "3DGS",
                                                       SHADER(3dgs.comp.glsl.spv),
                                                       (params.numPoints + 255) / 256,
                                                       1,
                                                       1);

    inputBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    result = inputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, maxPoints * sizeof(GaussianPoint3D));
    if (result != VK_SUCCESS) {
        Logger() << "Failed to allocate GPU buffer!" << std::endl;
        return false;
    }

    pixelMapBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    result = pixelMapBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                             params.width * params.height * 4);
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
            .bufferSize = pixelMapBuffer->GetBufferSize(),
            .buffer = pixelMapBuffer->GetBuffer()
        }
    });

    const PushConstantInfo pushConstantInfo{
        .size = sizeof(GaussianSplatting3DParams),
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

    computeSubGraph->AddComputeGraphNode(bezierNode);
    return true;
}

void GaussianSplatting3DRenderer::Render(const std::vector<GaussianPoint3D> &points) const {
    inputBuffer->UploadData(points.data(), points.size() * sizeof(GaussianPoint3D));

    const uint64_t time = TimeUtils::GetCurrentMonoMs();
    if (const VkResult ret = computeGraph->Compute(); ret != VK_SUCCESS) {
        Logger() << "Failed to compute graph!" << std::endl;
        return;
    }
    const uint64_t elapsed = TimeUtils::GetCurrentMonoMs() - time;
    Logger() << "TimeUsage: " << elapsed << "ms" << std::endl;
}

void GaussianSplatting3DRenderer::GeneratePixelMap(const std::string &path) const {
    if (const VkResult ret = pixelMapBuffer->MapBuffer(); ret != VK_SUCCESS) {
        Logger() << "Failed to map pixel buffer!" << std::endl;
        return;
    }
    ImageUtils::WritePngFile(path,
                             params.width,
                             params.height,
                             4,
                             pixelMapBuffer->GetMappedAddr());
    pixelMapBuffer->UnMapBuffer();
}
