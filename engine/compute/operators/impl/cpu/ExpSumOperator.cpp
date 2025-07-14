//
// Created by neo on 25-7-9.
//

#include "ExpSumOperator.h"

#include <cassert>

#include "core/log/Log.h"

CPUExpSumNode::CPUExpSumNode() {
    this->type = COMPUTE_GRAPH_NODE_CPU;
    this->name = "CPUExpSumNode";
}

VkResult CPUExpSumNode::CreateComputeGraphNode() {
    return VK_SUCCESS;
}

void CPUExpSumNode::Compute(const VkCommandBuffer commandBuffer) {
    CPUComputeNode::Compute(commandBuffer);

    assert(sum != nullptr);

    const float maxVal = max == nullptr ? 0.0f : *sum;

    *sum = 0;
    if (inputBuffer->MapBuffer(inputBuffer->GetBufferSize()) != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to map buffer!" << std::endl;
        *sum = 0.0f;
    }
    for (size_t i = 0; i < inputBuffer->GetBufferSize() / sizeof(float); i++) {
        *sum += exp((static_cast<float *>(inputBuffer->GetMappedAddr()))[i] - maxVal);
    }
    inputBuffer->UnMapBuffer();
}

void CPUExpSumNode::Destroy() {
    CPUComputeNode::Destroy();
}

ExpSumOperator::ExpSumOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer) {
    this->inputBuffer = inputBuffer;
}

std::shared_ptr<IComputeGraphNode> ExpSumOperator::CreateComputeGraphNode() {
    auto node = std::make_shared<CPUExpSumNode>();
    node->SetInputBuffer(inputBuffer);
    node->SetMax(max);
    if (sum != nullptr) {
        node->SetSum(sum);
    } else {
        node->SetSum(&innerSum);
    }
    return node;
}

void ExpSumOperator::Destroy() {
}
