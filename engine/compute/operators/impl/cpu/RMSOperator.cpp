//
// Created by neo on 25-6-27.
//

#include "RMSOperator.h"

#include "core/log/Log.h"

CPURmsNode::CPURmsNode() {
  this->type = COMPUTE_GRAPH_NODE_CPU;
  this->name = "CPURmsNode";
}

VkResult CPURmsNode::CreateComputeGraphNode() { return VK_SUCCESS; }

void CPURmsNode::Compute(const VkCommandBuffer commandBuffer) {
  CPUComputeNode::Compute(commandBuffer);

  if (inputBuffer->MapBuffer(inputBuffer->GetBufferSize()) != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to map buffer!" << std::endl;
    *rms = 0.0f;
  }
  float sum = 0.0f;
  const size_t nums = inputBuffer->GetBufferSize() / sizeof(float);
  for (size_t i = 0; i < nums; i++) {
    sum += pow(static_cast<float *>(inputBuffer->GetMappedAddr())[i], 2.0f);
  }
  inputBuffer->UnMapBuffer();

  *rms = sqrt(sum / static_cast<float>(nums) + bias);
}

void CPURmsNode::Destroy() { CPUComputeNode::Destroy(); }

RMSOperator::RMSOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer) {
  this->inputBuffer = inputBuffer;
}

std::shared_ptr<IComputeGraphNode> RMSOperator::CreateComputeGraphNode() {
  auto node = std::make_shared<CPURmsNode>();
  node->SetInputBuffer(inputBuffer);
  node->SetRms(&rms);
  node->SetBias(bias);
  return node;
}

void RMSOperator::Destroy() {}
