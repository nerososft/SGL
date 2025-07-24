//
// Created by neo on 25-7-3.
//

#include "Sequence.h"

#include "core/log/Log.h"

Sequence::Sequence(const std::shared_ptr<VkGPUContext> &gpuCtx) {
  this->gpuCtx = gpuCtx;
}

std::shared_ptr<Sequence>
Sequence::Record(const std::shared_ptr<IComputeGraphNode> &node) {
  if (node == nullptr) {
    return shared_from_this();
  }
  this->mainSubGraph->AddComputeGraphNode(node);
  return shared_from_this();
}

std::shared_ptr<Sequence> Sequence::Eval() {
  const VkResult result = this->graph->Compute();
  if (result != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to execute compute graph" << std::endl;
    return nullptr;
  }
  return shared_from_this();
}

std::shared_ptr<Sequence> Sequence::Destroy() {
  this->mainSubGraph->Destroy();
  return shared_from_this();
}

bool Sequence::Init() {
  this->mainSubGraph = std::make_shared<SubComputeGraph>(this->gpuCtx);
  if (this->mainSubGraph == nullptr) {
    Logger() << Logger::ERROR << "Failed to initialize SubComputeGraph!"
             << std::endl;
    return false;
  }
  const VkResult result = this->mainSubGraph->Init();
  if (result != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to initialize main SubComputeGraph!"
             << std::endl;
    return false;
  }

  this->graph = std::make_shared<ComputeGraph>(this->gpuCtx);
  if (this->graph == nullptr) {
    Logger() << Logger::ERROR << "Failed to create compute graph!" << std::endl;
    return false;
  }
  this->graph->AddSubGraph(this->mainSubGraph);
  return true;
}
