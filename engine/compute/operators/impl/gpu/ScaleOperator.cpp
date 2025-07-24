//
// Created by neo on 25-6-30.
//

#include "ScaleOperator.h"

#include "core/gpu/VkGPUHelper.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"

ScaleOperator::ScaleOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                             const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                             const std::shared_ptr<VkGPUBuffer> &outputBuffer)
    : UnaryOperator(gpuCtx, inputBuffer, outputBuffer) {}

ScaleOperator::~ScaleOperator() = default;

std::shared_ptr<IComputeGraphNode> ScaleOperator::CreateComputeGraphNode() {
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
  auto scaleNode = std::make_shared<ComputePipelineNode>(
      this->gpuCtx, "Scale", SHADER(scale_d.comp.glsl.spv), 0,
      descriptorSetLayoutBindings, (nums + 255) / 256, 1, 1);
  const VkResult ret = scaleNode->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << "Error creating scale node." << std::endl;
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

  const PushConstantInfo pushConstantInfo{.size = sizeof(ScaleOperatorParams),
                                          .data = &this->params};
  const ComputeElement computeElem{
      .pushConstantInfo = pushConstantInfo,
      .buffers = buffers,
      .customDrawFunc = nullptr,
  };
  scaleNode->AddComputeElement(computeElem);

  return scaleNode;
}

void ScaleOperator::Destroy() { UnaryOperator::Destroy(); }
