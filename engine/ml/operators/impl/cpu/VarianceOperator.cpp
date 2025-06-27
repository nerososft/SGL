//
// Created by neo on 25-6-27.
//

#include "VarianceOperator.h"

#include "CPUComputeNode.h"
#include "core/log/Log.h"

CPUVarianceNode::CPUVarianceNode() {
    this->type = COMPUTE_GRAPH_NODE_CPU;
    this->name = "CPUVarianceNode";
}

VkResult CPUVarianceNode::CreateComputeGraphNode() {
    return VK_SUCCESS;
}

void CPUVarianceNode::Compute(VkCommandBuffer commandBuffer) {
    CPUComputeNode::Compute(commandBuffer);
    if (avg == nullptr) {
        Logger() << "avg == nullptr";
        return;
    }
    if (inputBuffer->MapBuffer(inputBuffer->GetBufferSize()) != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to map buffer!" << std::endl;
        *variance = 0.0f;
    }
    float sum = 0.0f;
    const size_t nums = inputBuffer->GetBufferSize() / sizeof(float);
    for (size_t i = 0; i < nums; i++) {
        sum += pow(static_cast<float *>(inputBuffer->GetMappedAddr())[i] - *avg, 2.0);
    }
    inputBuffer->UnMapBuffer();

    *variance = sum / static_cast<float>(nums);
}

void CPUVarianceNode::Destroy() {
    IComputeGraphNode::Destroy();
}

VarianceOperator::VarianceOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer, float *avg) {
    this->inputBuffer = inputBuffer;
    this->avg = avg;
}

std::shared_ptr<IComputeGraphNode> VarianceOperator::CreateComputeGraphNode() {
    auto node = std::make_shared<CPUVarianceNode>();
    node->SetInputBuffer(inputBuffer);
    node->SetVariance(&variance);
    node->SetAvg(avg);
    return node;
}

void VarianceOperator::Destroy() {
}
