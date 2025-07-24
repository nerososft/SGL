//
// Created by 1234 on 2025/3/6.
//

#include "ColorBalanceFilter.h"

#include "core/config.h"

#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <core/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "core/gpu/VkGPUHelper.h"
#include "core/gpu/compute_graph/BufferCopyNode.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"
#include "engine/effect/filters/BasicFilter.h"

VkResult
ColorBalanceFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                          const std::vector<FilterImageInfo> &inputImageInfo,
                          const std::vector<FilterImageInfo> &outputImageInfo) {
  BasicFilterParams params;
  this->bFilterParams.imageSize.width = inputImageInfo[0].width;
  this->bFilterParams.imageSize.height = inputImageInfo[0].height;
  this->bFilterParams.imageSize.channels = 4;
  this->bFilterParams.imageSize.bytesPerLine =
      this->bFilterParams.imageSize.width * 4;

  this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
  this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);

  VkResult ret = this->computeSubGraph->Init();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  PushConstantInfo pushConstantInfo;
  pushConstantInfo.size = sizeof(colorBalanceFilterParams);
  pushConstantInfo.data = &this->bFilterParams;

  PBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
  PBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, pSize * sizeof(int));

  adjustPBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
  adjustPBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM,
                                 adjustPSize * sizeof(float));
  PBuffer->UploadData(P, pSize * sizeof(int));
  adjustPBuffer->UploadData(adjustP, adjustPSize * sizeof(float));

  PipelineNodeBuffer pipelineNodeInput;
  pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
  pipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
  pipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

  PipelineNodeBuffer pipelineNodeAdjustPInput;
  pipelineNodeAdjustPInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
  pipelineNodeAdjustPInput.buf.buffer = adjustPBuffer->GetBuffer();
  pipelineNodeAdjustPInput.buf.bufferSize = pSize * sizeof(float);

  PipelineNodeBuffer pipelineNodePInput;
  pipelineNodePInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
  pipelineNodePInput.buf.buffer = PBuffer->GetBuffer();
  pipelineNodePInput.buf.bufferSize = pSize * sizeof(int);

  PipelineNodeBuffer pipelineNodeOutput;
  pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
  pipelineNodeOutput.buf.buffer = outputImageInfo[0].storageBuffer;
  pipelineNodeOutput.buf.bufferSize = outputImageInfo[0].bufferSize;

  std::vector<PipelineNodeBuffer> vPipelineBuffers;
  vPipelineBuffers.push_back(pipelineNodeInput);
  vPipelineBuffers.push_back(pipelineNodeAdjustPInput);
  vPipelineBuffers.push_back(pipelineNodePInput);
  vPipelineBuffers.push_back(pipelineNodeOutput);

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
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
          VK_SHADER_STAGE_COMPUTE_BIT));

  const auto kCalculateNode = std::make_shared<ComputePipelineNode>(
      gpuCtx, "colorBalance", SHADER(color_balance.comp.glsl.spv),
      pushConstantInfo.size, descriptorSetLayoutBindings,
      (inputImageInfo[0].width + 31) / 32, (inputImageInfo[0].height + 31) / 32,
      1);

  ret = kCalculateNode->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  kCalculateNode->AddComputeElement(
      {.pushConstantInfo = pushConstantInfo, .buffers = vPipelineBuffers});

  computeSubGraph->AddComputeGraphNode(kCalculateNode);
  computeGraph->AddSubGraph(computeSubGraph);

  return computeGraph->Compute();
}

void ColorBalanceFilter::Destroy() {
  if (computeGraph != nullptr) {
    computeGraph->Destroy();
    computeGraph = nullptr;
  }
  if (PBuffer != nullptr) {
    PBuffer->Destroy();
    PBuffer = nullptr;
  }
  if (adjustPBuffer != nullptr) {
    adjustPBuffer->Destroy();
    adjustPBuffer = nullptr;
  }
}
