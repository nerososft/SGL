//
// Created by neo on 25-6-30.
//

#include "MulOperator.h"

#include "runtime/config.h"
#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/gpu/compute_graph/ComputePipelineNode.h"
#include "runtime/log/Log.h"

MulOperator::MulOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                         const std::shared_ptr<VkGPUBuffer> &inputBuffer1,
                         const std::shared_ptr<VkGPUBuffer> &inputBuffer2,
                         const std::shared_ptr<VkGPUBuffer> &outputBuffer)
    : BinaryOperator(gpuCtx, inputBuffer1, inputBuffer2, outputBuffer) {}

MulOperator::~MulOperator() = default;

std::shared_ptr<IComputeGraphNode> MulOperator::CreateComputeGraphNode() {
  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
          VK_SHADER_STAGE_COMPUTE_BIT));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
          VK_SHADER_STAGE_COMPUTE_BIT));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
          VK_SHADER_STAGE_COMPUTE_BIT));
  const size_t nums = outputBuffer->GetBufferSize() / sizeof(float);
  auto mulNode = std::make_shared<ComputePipelineNode>(
      this->gpuCtx, "ElementWiseMultiplication", SHADER(mul.comp.glsl.spv), 0,
      descriptorSetLayoutBindings, (nums + 255) / 256, 1, 1);
  const VkResult ret = mulNode->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << "Error creating Element-wise Multiplication node." << std::endl;
    return nullptr;
  }
  std::vector<PipelineNodeBuffer> buffers;
  buffers.push_back({.type = PIPELINE_NODE_BUFFER_STORAGE_READ,
                     .buf = {
                         .bufferSize = this->inputBuffer1->GetBufferSize(),
                         .buffer = this->inputBuffer1->GetBuffer(),
                     }});
  buffers.push_back({.type = PIPELINE_NODE_BUFFER_STORAGE_READ,
                     .buf = {
                         .bufferSize = this->inputBuffer2->GetBufferSize(),
                         .buffer = this->inputBuffer2->GetBuffer(),
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
  mulNode->AddComputeElement(computeElem);

  return mulNode;
}

void MulOperator::Destroy() { BinaryOperator::Destroy(); }
