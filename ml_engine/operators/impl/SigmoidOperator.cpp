//
// Created by neo on 25-5-25.
//

#include "SigmoidOperator.h"

#include "gpu_engine/config.h"
#include "gpu_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "gpu_engine/log/Log.h"

SigmoidOperator::SigmoidOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                 const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                                 const std::shared_ptr<VkGPUBuffer> &outputBuffer)
    : UnaryOperator(gpuCtx, inputBuffer, outputBuffer) {
}

SigmoidOperator::~SigmoidOperator() {
}

std::shared_ptr<IComputeGraphNode> SigmoidOperator::CreateComputeGraphNode() {
    const size_t nums = outputBuffer->GetBufferSize() / sizeof(float);
    auto sigmoidNode = std::make_shared<ComputePipelineNode>(this->gpuCtx,
                                                             "Sigmoid",
                                                             SHADER(sigmoid.comp.glsl.spv),
                                                             (nums + 255) / 256,
                                                             1,
                                                             1);
    std::vector<PipelineNodeBuffer> buffers;
    buffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_READ,
        .bufferSize = this->inputBuffer->GetBufferSize(),
        .buffer = this->inputBuffer->GetBuffer(),
    });
    buffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
        .bufferSize = this->outputBuffer->GetBufferSize(),
        .buffer = this->outputBuffer->GetBuffer(),
    });

    const PushConstantInfo pushConstantInfo{};
    const ComputeElement computeElem{
        .pushConstantInfo = pushConstantInfo,
        .buffers = buffers,
        .customDrawFunc = nullptr,
    };
    sigmoidNode->AddComputeElement(computeElem);

    const VkResult ret = sigmoidNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Error creating sigmoid node." << std::endl;
        return nullptr;
    }
    return sigmoidNode;
}

void SigmoidOperator::Destroy() {
    UnaryOperator::Destroy();
}
