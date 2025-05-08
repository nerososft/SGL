//
// Created by neo on 25-5-8.
//

#include "MatMulOperator.h"

#include "gpu_engine/config.h"
#include "gpu_engine/gpu/compute_graph/ComputePipelineNode.h"
#include "gpu_engine/log/Log.h"

MatMulOperator::MatMulOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                               const std::shared_ptr<VkGPUBuffer> &inputBuffer1,
                               const std::shared_ptr<VkGPUBuffer> &inputBuffer2,
                               const std::shared_ptr<VkGPUBuffer> &outputBuffer)
    : BinaryOperator(gpuCtx, inputBuffer1, inputBuffer2, outputBuffer) {
}

std::shared_ptr<IComputeGraphNode> MatMulOperator::CreateComputeGraphNode() {
    auto gemmNode = std::make_shared<ComputePipelineNode>(this->gpuCtx,
                                                          "MatMul",
                                                          SHADER(matmul.comp.glsl.spv),
                                                          32, // FIXME: (width + 31) / 32
                                                          32,
                                                          1);

    std::vector<PipelineNodeBuffer> buffers;
    buffers.push_back({
        .buffer = this->inputBuffer1->GetBuffer(),
        .type = PIPELINE_NODE_BUFFER_STORAGE_READ,
        .bufferSize = this->inputBuffer1->GetBufferSize()
    });
    buffers.push_back({
        .buffer = this->inputBuffer2->GetBuffer(),
        .type = PIPELINE_NODE_BUFFER_STORAGE_READ,
        .bufferSize = this->inputBuffer2->GetBufferSize()
    });
    buffers.push_back({
        .buffer = this->outputBuffer->GetBuffer(),
        .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
        .bufferSize = this->outputBuffer->GetBufferSize()
    });

    const PushConstantInfo pushConstantInfo{};
    const ComputeElement computeElem{
        .buffers = buffers,
        .pushConstantInfo = pushConstantInfo,
        .customDrawFunc = nullptr,
    };
    gemmNode->AddComputeElement(computeElem);

    const VkResult ret = gemmNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Error creating gemm node." << std::endl;
        return nullptr;
    }
    return gemmNode;
}

MatMulOperator::~MatMulOperator() {
    this->Destroy();
}

void MatMulOperator::Destroy() {
    BinaryOperator::Destroy();
}
