//
// Created by neo on 25-5-25.
//

#include "GELUOperator.h"

#include "core/config.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"

GELUOperator::GELUOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                           const std::shared_ptr<VkGPUBuffer> &outputBuffer)
    : UnaryOperator(gpuCtx, inputBuffer, outputBuffer) {
}

GELUOperator::~GELUOperator() {
}

std::shared_ptr<IComputeGraphNode> GELUOperator::CreateComputeGraphNode() {
    const size_t nums = outputBuffer->GetBufferSize() / sizeof(float);
    auto geluNode = std::make_shared<ComputePipelineNode>(this->gpuCtx,
                                                          "GELU",
                                                          SHADER(gelu.comp.glsl.spv),
                                                          (nums + 255) / 256,
                                                          1,
                                                          1);
    std::vector<PipelineNodeBuffer> buffers;
    buffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_READ,
        .buf = {
            .bufferSize = this->inputBuffer->GetBufferSize(),
            .buffer = this->inputBuffer->GetBuffer(),
        }
    });
    buffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
        .buf = {
            .bufferSize = this->outputBuffer->GetBufferSize(),
            .buffer = this->outputBuffer->GetBuffer(),
        }
    });

    const PushConstantInfo pushConstantInfo{};
    const ComputeElement computeElem{
        .pushConstantInfo = pushConstantInfo,
        .buffers = buffers,
        .customDrawFunc = nullptr,
    };
    geluNode->AddComputeElement(computeElem);

    const VkResult ret = geluNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Error creating gelu node." << std::endl;
        return nullptr;
    }
    return geluNode;
}

void GELUOperator::Destroy() {
    UnaryOperator::Destroy();
}
