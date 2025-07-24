//
// Created by neo on 25-6-27.
//

#include "CPUComputeNode.h"

#include "core/log/Log.h"

void CPUComputeNode::Compute(const VkCommandBuffer commandBuffer) {
  if (!this->dependencies.empty()) {
    for (const auto &dependence : this->dependencies) {
      Logger() << Logger::DEBUG << "Node: " << name
               << " Depend On:" << dependence->GetName() << std::endl;
      dependence->Compute(commandBuffer);
    }
  }
  Logger() << Logger::DEBUG << "Executing Compute Node: " << name << std::endl;
}

void CPUComputeNode::Destroy() { IComputeGraphNode::Destroy(); }

VkResult CPUComputeNode::CreateComputeGraphNode() { return VK_SUCCESS; }
