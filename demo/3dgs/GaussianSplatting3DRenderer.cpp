//
// Created by neo on 25-6-3.
//

#include "GaussianSplatting3DRenderer.h"

#include "core/gpu/VkGPUHelper.h"
#include "core/gpu/compute_graph/ComputeGraph.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"
#include "core/utils/ImageUtils.h"
#include "core/utils/TimeUtils.h"
#include "demo/compute_demo/GPUBezierThickLineGenerator.h"

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

bool GaussianSplatting3DRenderer::InitializeGPUPipeline(const size_t maxPoints) {
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

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    bezierNode = std::make_shared<ComputePipelineNode>(gpuCtx, "3DGS",
                                                       SHADER(3dgs.comp.glsl.spv),
                                                       sizeof(GaussianSplatting3DParams),
                                                       descriptorSetLayoutBindings,
                                                       (params.numPoints + 255) / 256,
                                                       1,
                                                       1);

    result = bezierNode->CreateComputeGraphNode();
    if (result != VK_SUCCESS) {
        Logger() << "Failed to create compute graph node!" << std::endl;
        return false;
    }

    inputBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    result = inputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, maxPoints * sizeof(GaussianPoint));
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

    depthMapBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    result = depthMapBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                             params.width * params.height * sizeof(float));
    if (result != VK_SUCCESS) {
        Logger() << "Failed to allocate GPU buffer!" << std::endl;
        return false;
    }
    result = depthMapBuffer->MapBuffer();
    if (result != VK_SUCCESS) {
        Logger() << "Failed to map GPU buffer!" << std::endl;
        return false;
    }
    auto *addr = static_cast<float *>(depthMapBuffer->GetMappedAddr());
    for (uint32_t i = 0; i < params.width * params.height; i++) {
        addr[i] = 999999.0f;
    }
    depthMapBuffer->UnMapBuffer();

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
    ppBuffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
        .buf = {
            .bufferSize = depthMapBuffer->GetBufferSize(),
            .buffer = depthMapBuffer->GetBuffer()
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

    computeSubGraph->AddComputeGraphNode(bezierNode);
    return true;
}

void GaussianSplatting3DRenderer::Render(const std::vector<GaussianPoint> &points) const {
    inputBuffer->UploadData(points.data(), points.size() * sizeof(GaussianPoint));

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
