//
// Created by neo on 25-5-8.
//

#include "MatMulOperator.h"

#include "runtime/config.h"
#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/gpu/compute_graph/ComputePipelineNode.h"
#include "runtime/log/Log.h"

MatMulOperator::MatMulOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                               const std::shared_ptr<VkGPUBuffer> &inputBuffer1,
                               const std::shared_ptr<VkGPUBuffer> &inputBuffer2,
                               const std::shared_ptr<VkGPUBuffer> &outputBuffer)
    : BinaryOperator(gpuCtx, inputBuffer1, inputBuffer2, outputBuffer) {}

std::shared_ptr<IComputeGraphNode> MatMulOperator::CreateComputeGraphNode() {
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
  auto gemmNode = std::make_shared<ComputePipelineNode>(
      this->gpuCtx, "MatMul", SHADER(matmul.comp.glsl.spv),
      sizeof(MatMulOperatorParams), descriptorSetLayoutBindings,
      (this->params.height1 + 31) / 32, (this->params.width2 + 31) / 32, 1);
  const VkResult ret = gemmNode->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << "Error creating gemm node." << std::endl;
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

  const PushConstantInfo pushConstantInfo{
      .size = sizeof(MatMulOperatorParams),
      .data = &this->params,
  };
  const ComputeElement computeElem{
      .pushConstantInfo = pushConstantInfo,
      .buffers = buffers,
      .customDrawFunc = nullptr,
  };
  gemmNode->AddComputeElement(computeElem);
  return gemmNode;
}

MatMulOperator::~MatMulOperator() { this->Destroy(); }

void MatMulOperator::Destroy() { BinaryOperator::Destroy(); }
