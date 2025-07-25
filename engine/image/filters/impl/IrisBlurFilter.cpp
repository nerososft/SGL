#include "IrisBlurFilter.h"

#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <core/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif
#include "engine/image/filters/BasicFilter.h"
#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/gpu/compute_graph/BufferCopyNode.h"
#include "runtime/gpu/compute_graph/ComputePipelineNode.h"
#include "runtime/log/Log.h"

VkResult
IrisBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                      const std::vector<FilterImageInfo> &inputImageInfo,
                      const std::vector<FilterImageInfo> &outputImageInfo) {
  BasicFilterParams params;
  this->irisblurFilterParams.imageSize.width = inputImageInfo[0].width;
  this->irisblurFilterParams.imageSize.height = inputImageInfo[0].height;
  this->irisblurFilterParams.imageSize.channels = 4;
  this->irisblurFilterParams.imageSize.bytesPerLine =
      this->irisblurFilterParams.imageSize.width * 4;

  this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
  this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
  VkResult ret = this->computeSubGraph->Init();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  PushConstantInfo pushConstantInfo;
  pushConstantInfo.size = sizeof(IrisBlurFilterParams);
  pushConstantInfo.data = &this->irisblurFilterParams;

  PipelineNodeBuffer pipelineNodeInput;
  pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
  pipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
  pipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

  PipelineNodeBuffer pipelineNodeTempInput;
  pipelineNodeTempInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
  pipelineNodeTempInput.buf.buffer = inputImageInfo[1].storageBuffer;
  pipelineNodeTempInput.buf.bufferSize = inputImageInfo[1].bufferSize;

  PipelineNodeBuffer pipelineNodeOutput;
  pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
  pipelineNodeOutput.buf.buffer = outputImageInfo[0].storageBuffer;
  pipelineNodeOutput.buf.bufferSize = outputImageInfo[0].bufferSize;

  std::vector<PipelineNodeBuffer> vPipelineBuffers;
  vPipelineBuffers.push_back(pipelineNodeInput);
  vPipelineBuffers.push_back(pipelineNodeTempInput);
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

  const auto kCalculateNode = std::make_shared<ComputePipelineNode>(
      gpuCtx, "irisblur", SHADER(irisblur.comp.glsl.spv), pushConstantInfo.size,
      descriptorSetLayoutBindings, (inputImageInfo[0].width + 31) / 32,
      (inputImageInfo[0].height + 31) / 32, 1);
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

void IrisBlurFilter::Destroy() {
  if (computeGraph != nullptr) {
    computeGraph->Destroy();
    computeGraph = nullptr;
  }
}
