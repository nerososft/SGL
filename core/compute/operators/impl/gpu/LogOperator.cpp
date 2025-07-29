//
// Created by neo on 25-7-29.
//

#include "LogOperator.h"

#include "runtime/config.h"
#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/gpu/compute_graph/ComputePipelineNode.h"
#include "runtime/log/Log.h"

LogOperator::LogOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                         const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                         const std::shared_ptr<VkGPUBuffer> &outputBuffer)
    : UnaryOperator(gpuCtx, inputBuffer, outputBuffer) {}

LogOperator::~LogOperator() = default;

std::shared_ptr<IComputeGraphNode> LogOperator::CreateComputeGraphNode() {
  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
          VK_SHADER_STAGE_COMPUTE_BIT));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
          VK_SHADER_STAGE_COMPUTE_BIT));
  const size_t nums = outputBuffer->GetBufferSize() / sizeof(float);
  auto logNode = std::make_shared<ComputePipelineNode>(
      this->gpuCtx, "Log", SHADER(log.comp.glsl.spv), 0,
      descriptorSetLayoutBindings, (nums + 255) / 256, 1, 1);
  const VkResult ret = logNode->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << "Error creating log node." << std::endl;
    return nullptr;
  }
  std::vector<PipelineNodeBuffer> buffers;
  buffers.push_back({.type = PIPELINE_NODE_BUFFER_STORAGE_READ,
                     .buf = {
                         .bufferSize = this->inputBuffer->GetBufferSize(),
                         .buffer = this->inputBuffer->GetBuffer(),
                     }});
  buffers.push_back({.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
                     .buf = {
                         .bufferSize = this->outputBuffer->GetBufferSize(),
                         .buffer = this->outputBuffer->GetBuffer(),
                     }});

  const PushConstantInfo pushConstantInfo{};
  const ComputeElement computeElem{
      .pushConstantInfo = pushConstantInfo,
      .buffers = buffers,
      .customDrawFunc = nullptr,
  };
  logNode->AddComputeElement(computeElem);

  return logNode;
}

void LogOperator::Destroy() { UnaryOperator::Destroy(); }
