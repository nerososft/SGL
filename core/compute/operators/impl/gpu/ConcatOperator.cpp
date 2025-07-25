//
// Created by neo on 25-6-28.
//

#include "ConcatOperator.h"

#include "runtime/config.h"
#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/gpu/compute_graph/ComputePipelineNode.h"
#include "runtime/log/Log.h"

ConcatOperator::ConcatOperator(
    const std::shared_ptr<VkGPUContext> &gpuCtx,
    const std::vector<std::shared_ptr<VkGPUBuffer>> &inputBuffers,
    const std::shared_ptr<VkGPUBuffer> &outBuffer)
    : Operator(gpuCtx) {
  this->inputBuffers = inputBuffers;
  this->outBuffer = outBuffer;
}

ConcatOperator::~ConcatOperator() = default;

std::shared_ptr<IComputeGraphNode> ConcatOperator::CreateComputeGraphNode() {
  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
  for (size_t i = 0; i < this->params.nums; ++i) {
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(
            i, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
            VK_SHADER_STAGE_COMPUTE_BIT));
  }
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          this->params.nums, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
          VK_SHADER_STAGE_COMPUTE_BIT));

  std::string shaderSpv = SHADER(concat16.comp.glsl.spv);
  if (params.nums == 8) {
    shaderSpv = SHADER(concat8.comp.glsl.spv);
  }

  const auto concatNode = std::make_shared<ComputePipelineNode>(
      this->gpuCtx, "Concat", shaderSpv, 0, descriptorSetLayoutBindings,
      (this->params.width + 31) / 32, (this->params.height + 31) / 32, 1);
  const VkResult ret = concatNode->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << "Error creating concat node." << std::endl;
    return nullptr;
  }
  std::vector<PipelineNodeBuffer> buffers;
  for (size_t i = 0; i < this->params.nums; ++i) {
    buffers.push_back({.type = PIPELINE_NODE_BUFFER_STORAGE_READ,
                       .buf = {
                           .bufferSize = this->inputBuffers[i]->GetBufferSize(),
                           .buffer = this->inputBuffers[i]->GetBuffer(),
                       }});
  }
  buffers.push_back({.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
                     .buf = {
                         .bufferSize = this->outBuffer->GetBufferSize(),
                         .buffer = this->outBuffer->GetBuffer(),
                     }});

  const PushConstantInfo pushConstantInfo{
      .size = sizeof(ConcatOperatorParams),
      .data = &this->params,
  };
  const ComputeElement computeElem{
      .pushConstantInfo = pushConstantInfo,
      .buffers = buffers,
      .customDrawFunc = nullptr,
  };
  concatNode->AddComputeElement(computeElem);

  return concatNode;
}

void ConcatOperator::Destroy() { Operator::Destroy(); }
