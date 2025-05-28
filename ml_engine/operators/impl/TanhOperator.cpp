//
// Created by neo on 25-5-25.
//

#include "TanhOperator.h"

#include "gpu_engine/config.h"
#include "gpu_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "gpu_engine/log/Log.h"

TanhOperator::TanhOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                           const std::shared_ptr<VkGPUBuffer> &outputBuffer)
    : UnaryOperator(gpuCtx, inputBuffer, outputBuffer) {
}

TanhOperator::~TanhOperator() {
}

std::shared_ptr<IComputeGraphNode> TanhOperator::CreateComputeGraphNode() {
    const size_t nums = outputBuffer->GetBufferSize() / sizeof(float);
    auto tanhNode = std::make_shared<ComputePipelineNode>(this->gpuCtx,
                                                          "Tanh",
                                                          SHADER(tanh.comp.glsl.spv),
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
    tanhNode->AddComputeElement(computeElem);

    const VkResult ret = tanhNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Error creating tanh node." << std::endl;
        return nullptr;
    }
    return tanhNode;
}

void TanhOperator::Destroy() {
    UnaryOperator::Destroy();
}
