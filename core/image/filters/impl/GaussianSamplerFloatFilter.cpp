//
// Created by neo on 2025/3/13.
//

#include "GaussianSamplerFloatFilter.h"

#include "runtime/config.h"

#include "runtime/gpu/compute_graph/BufferCopyNode.h"
#include "runtime/gpu/compute_graph/ComputePipelineNode.h"
#ifdef OS_OPEN_HARMONY
#include <runtime/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/log/Log.h"

#include <assert.h>

VkResult GaussianSamplerFloatFilter::Apply(
    const std::shared_ptr<VkGPUContext> &gpuCtx,
    const std::vector<FilterImageInfo> &inputImageInfo,
    const std::vector<FilterImageInfo> &outputImageInfo) {
  assert(inputImageInfo[0].storageBuffer != VK_NULL_HANDLE);
  assert(outputImageInfo[0].storageBuffer != VK_NULL_HANDLE);
  assert(inputImageInfo[0].bufferSize > 0);
  assert(outputImageInfo[0].bufferSize > 0);

  BasicFilterParams params;
  this->gaussianSamplerFilterParams.imageSize.width = inputImageInfo[0].width;
  this->gaussianSamplerFilterParams.imageSize.height = inputImageInfo[0].height;
  this->gaussianSamplerFilterParams.imageSize.channels = 1;
  this->gaussianSamplerFilterParams.imageSize.bytesPerLine =
      this->gaussianSamplerFilterParams.imageSize.width * 4;

  this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
  this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);

  VkResult ret = this->computeSubGraph->Init();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  PushConstantInfo pushConstantInfo;
  pushConstantInfo.size = sizeof(GaussianSamplerFilterParams);
  pushConstantInfo.data = &this->gaussianSamplerFilterParams;

  PipelineNodeBuffer pipelineNodeInput;
  pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
  pipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
  pipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

  PipelineNodeBuffer pipelineNodeOutput;
  pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
  pipelineNodeOutput.buf.buffer = outputImageInfo[0].storageBuffer;
  pipelineNodeOutput.buf.bufferSize = outputImageInfo[0].bufferSize;

  std::vector<PipelineNodeBuffer> vPipelineBuffers;
  vPipelineBuffers.push_back(pipelineNodeInput);
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

  const auto gaussianSamplerNode = std::make_shared<ComputePipelineNode>(
      gpuCtx, "gaussianSamplerFloatImage",
      SHADER(gaussian_sampler_float.comp.glsl.spv), pushConstantInfo.size,
      descriptorSetLayoutBindings, (outputImageInfo[0].width + 31) / 32,
      (outputImageInfo[0].height + 31) / 32, 1);

  ret = gaussianSamplerNode->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  gaussianSamplerNode->AddComputeElement(
      {.pushConstantInfo = pushConstantInfo, .buffers = vPipelineBuffers});

  computeSubGraph->AddComputeGraphNode(gaussianSamplerNode);
  computeGraph->AddSubGraph(computeSubGraph);

  return computeGraph->Compute();
}

void GaussianSamplerFloatFilter::Destroy() { BasicFilter::Destroy(); }
