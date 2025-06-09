//
// Created by neo on 25-5-7.
//

#include "ReLUOperator.h"

#include "core/config.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"

ReLUOperator::ReLUOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                           const std::shared_ptr<VkGPUBuffer> &outputBuffer)
    : UnaryOperator(gpuCtx, inputBuffer, outputBuffer) {
}

ReLUOperator::~ReLUOperator() {
}

std::shared_ptr<IComputeGraphNode> ReLUOperator::CreateComputeGraphNode() {
    const size_t nums = outputBuffer->GetBufferSize() / sizeof(float);
    auto reluNode = std::make_shared<ComputePipelineNode>(this->gpuCtx,
                                                          "ReLU",
                                                          SHADER(relu.comp.glsl.spv),
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
    reluNode->AddComputeElement(computeElem);

    const VkResult ret = reluNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Error creating relu node." << std::endl;
        return nullptr;
    }
    return reluNode;
}

void ReLUOperator::Destroy() {
    UnaryOperator::Destroy();
}
