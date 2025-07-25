//
// Created by neo on 25-6-26.
//

#include "AvgOperator.h"

#include "runtime/log/Log.h"

CPUAvgNode::CPUAvgNode() {
  this->type = COMPUTE_GRAPH_NODE_CPU;
  this->name = "CPUAvgNode";
}

VkResult CPUAvgNode::CreateComputeGraphNode() { return VK_SUCCESS; }

void CPUAvgNode::Compute(const VkCommandBuffer commandBuffer) {
  CPUComputeNode::Compute(commandBuffer);

  if (inputBuffer->MapBuffer(inputBuffer->GetBufferSize()) != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to map buffer!" << std::endl;
    *avg = 0.0f;
  }
  float sum = 0.0f;
  const size_t nums = inputBuffer->GetBufferSize() / sizeof(float);
  for (size_t i = 0; i < nums; i++) {
    sum += static_cast<float *>(inputBuffer->GetMappedAddr())[i];
  }
  inputBuffer->UnMapBuffer();
  *avg = sum / static_cast<float>(nums);
}

void CPUAvgNode::Destroy() { CPUComputeNode::Destroy(); }

AvgOperator::AvgOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer) {
  this->inputBuffer = inputBuffer;
}

std::shared_ptr<IComputeGraphNode> AvgOperator::CreateComputeGraphNode() {
  auto node = std::make_shared<CPUAvgNode>();
  node->SetInputBuffer(inputBuffer);
  node->SetAvg(&avg);
  return node;
}

void AvgOperator::Destroy() {}
