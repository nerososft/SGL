//
// Created by neo on 25-6-27.
//

#include "SumOperator.h"

#include "core/log/Log.h"

CPUSumNode::CPUSumNode() {
    this->type = COMPUTE_GRAPH_NODE_CPU;
    this->name = "CPUSumNode";
}

VkResult CPUSumNode::CreateComputeGraphNode() {
    return VK_SUCCESS;
}

void CPUSumNode::Compute(const VkCommandBuffer commandBuffer) {
    CPUComputeNode::Compute(commandBuffer);

    *sum = 0;
    if (inputBuffer->MapBuffer(inputBuffer->GetBufferSize()) != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to map buffer!" << std::endl;
        *sum = 0.0f;
    }
    for (size_t i = 0; i < inputBuffer->GetBufferSize() / sizeof(float); i++) {
        *sum += (static_cast<float *>(inputBuffer->GetMappedAddr()))[i];
    }
    inputBuffer->UnMapBuffer();
}

void CPUSumNode::Destroy() {
    CPUComputeNode::Destroy();
}

SumOperator::SumOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer) {
    this->inputBuffer = inputBuffer;
}

std::shared_ptr<IComputeGraphNode> SumOperator::CreateComputeGraphNode() {
    auto node = std::make_shared<CPUSumNode>();
    node->SetInputBuffer(inputBuffer);
    if (sum != nullptr) {
        node->SetSum(sum);
    } else {
        node->SetSum(&innerSum);
    }
    return node;
}

void SumOperator::Destroy() {
}
