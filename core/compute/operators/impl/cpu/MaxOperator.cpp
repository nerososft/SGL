//
// Created by neo on 25-7-9.
//

#include "MaxOperator.h"

#include "runtime/log/Log.h"

CPUMaxNode::CPUMaxNode() {
  this->type = COMPUTE_GRAPH_NODE_CPU;
  this->name = "CPUMaxNode";
}

VkResult CPUMaxNode::CreateComputeGraphNode() { return VK_SUCCESS; }

void CPUMaxNode::Compute(const VkCommandBuffer commandBuffer) {
  CPUComputeNode::Compute(commandBuffer);

  std::vector<float> data(inputBuffer->GetBufferSize() / sizeof(float));
  inputBuffer->DownloadData(data.data(), inputBuffer->GetBufferSize());
  std::ranges::sort(data, std::greater<float>());
  *max = data[0];
}

void CPUMaxNode::Destroy() { CPUComputeNode::Destroy(); }

MaxOperator::MaxOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer) {
  this->inputBuffer = inputBuffer;
}

std::shared_ptr<IComputeGraphNode> MaxOperator::CreateComputeGraphNode() {
  auto node = std::make_shared<CPUMaxNode>();
  node->SetInputBuffer(inputBuffer);
  if (max != nullptr) {
    node->SetMax(max);
  } else {
    node->SetMax(&innerMax);
  }
  return node;
}

void MaxOperator::Destroy() {}
