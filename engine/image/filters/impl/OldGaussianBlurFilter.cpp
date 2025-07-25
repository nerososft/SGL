//
// Created by neo on 2025/3/11.
//

#include "OldGaussianBlurFilter.h"

#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <core/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif
#include "core/config.h"
#include "core/gpu/VkGPUHelper.h"
#include "core/gpu/compute_graph/BufferCopyNode.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"
#include "engine/image/filters/BasicFilter.h"

VkResult OldGaussianBlurFilter::Apply(
    const std::shared_ptr<VkGPUContext> &gpuCtx,
    const std::vector<FilterImageInfo> &inputImageInfo,
    const std::vector<FilterImageInfo> &outputImageInfo) {
  this->blurFilterParams.imageSize.width = inputImageInfo[0].width;
  this->blurFilterParams.imageSize.height = inputImageInfo[0].height;
  this->blurFilterParams.imageSize.channels = 4;
  this->blurFilterParams.imageSize.bytesPerLine =
      this->blurFilterParams.imageSize.width * 4;

  this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
  this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
  VkResult ret = this->computeSubGraph->Init();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  PushConstantInfo pushConstantInfo;
  pushConstantInfo.size = sizeof(OldGaussianBlurFilterParams);
  pushConstantInfo.data = &this->blurFilterParams;

  PipelineNodeBuffer vPipelineNodeInput;
  vPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
  vPipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
  vPipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

  PipelineNodeBuffer vPipelineNodeOutput;
  vPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
  vPipelineNodeOutput.buf.buffer = outputImageInfo[0].storageBuffer;
  vPipelineNodeOutput.buf.bufferSize = outputImageInfo[0].bufferSize;

  std::vector<PipelineNodeBuffer> vPipelineBuffers;
  vPipelineBuffers.push_back(vPipelineNodeInput);
  vPipelineBuffers.push_back(vPipelineNodeOutput);

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
          VK_SHADER_STAGE_COMPUTE_BIT));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
          VK_SHADER_STAGE_COMPUTE_BIT));

  const auto gaussianVerticalNode = std::make_shared<ComputePipelineNode>(
      gpuCtx, "OldGaussianVerticalBlur",
      SHADER(vertical_blur_old.comp.glsl.spv), pushConstantInfo.size,
      descriptorSetLayoutBindings, (inputImageInfo[0].width + 31) / 32,
      (inputImageInfo[0].height + 31) / 32, 1);
  ret = gaussianVerticalNode->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  gaussianVerticalNode->AddComputeElement(
      {.pushConstantInfo = pushConstantInfo, .buffers = vPipelineBuffers});

  PipelineNodeBuffer hPipelineNodeInput;
  hPipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
  hPipelineNodeInput.buf.buffer = outputImageInfo[0].storageBuffer;
  hPipelineNodeInput.buf.bufferSize = outputImageInfo[0].bufferSize;

  PipelineNodeBuffer hPipelineNodeOutput;
  hPipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
  hPipelineNodeOutput.buf.buffer = inputImageInfo[0].storageBuffer;
  hPipelineNodeOutput.buf.bufferSize = inputImageInfo[0].bufferSize;

  std::vector<PipelineNodeBuffer> hPipelineBuffers;
  hPipelineBuffers.push_back(hPipelineNodeInput);
  hPipelineBuffers.push_back(hPipelineNodeOutput);

  const auto gaussianHorizontalNode = std::make_shared<ComputePipelineNode>(
      gpuCtx, "OldGaussianHorizontalBlur",
      SHADER(horizontal_blur_old.comp.glsl.spv), pushConstantInfo.size,
      descriptorSetLayoutBindings, (outputImageInfo[0].width + 31) / 32,
      (outputImageInfo[0].height + 31) / 32, 1);

  ret = gaussianHorizontalNode->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  gaussianHorizontalNode->AddComputeElement(
      {.pushConstantInfo = pushConstantInfo, .buffers = hPipelineBuffers});

  BufferCopyNodeBufferInfo srcBufferInfo;
  srcBufferInfo.buffer = inputImageInfo[0].storageBuffer;
  srcBufferInfo.bufferSize = inputImageInfo[0].bufferSize;
  BufferCopyNodeBufferInfo dstBufferInfo;
  dstBufferInfo.buffer = outputImageInfo[0].storageBuffer;
  dstBufferInfo.bufferSize = outputImageInfo[0].bufferSize;
  const auto copyBufferNode = std::make_shared<BufferCopyNode>(
      gpuCtx, "CopyBufferToOutputBuffer", srcBufferInfo, dstBufferInfo);
  copyBufferNode->CreateComputeGraphNode();

  copyBufferNode->AddDependenceNode(gaussianVerticalNode);
  copyBufferNode->AddDependenceNode(gaussianHorizontalNode);
  computeSubGraph->AddComputeGraphNode(copyBufferNode);
  computeGraph->AddSubGraph(computeSubGraph);

  return computeGraph->Compute();
}

void OldGaussianBlurFilter::Destroy() {
  if (computeGraph != nullptr) {
    computeGraph->Destroy();
    computeGraph = nullptr;
  }
}
