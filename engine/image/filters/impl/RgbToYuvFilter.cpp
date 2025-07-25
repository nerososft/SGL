//
// Created by neo on 25-7-8.
//

#include "RgbToYuvFilter.h"

#include <runtime/config.h>
#include <runtime/gpu/VkGPUHelper.h>
#include <runtime/gpu/compute_graph/ComputePipelineNode.h>
#include <runtime/log/Log.h>
#ifdef OS_OPEN_HARMONY
#include <core/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

VkResult
RgbToYuvFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                      const std::vector<FilterImageInfo> &inputImageInfo,
                      const std::vector<FilterImageInfo> &outputImageInfo) {
  BasicFilterParams params;
  this->rgbToYuvFilterParams.imageSize.width = inputImageInfo[0].width;
  this->rgbToYuvFilterParams.imageSize.height = inputImageInfo[0].height;
  this->rgbToYuvFilterParams.imageSize.channels = 4;
  this->rgbToYuvFilterParams.imageSize.bytesPerLine =
      this->rgbToYuvFilterParams.imageSize.width * 4;
  params.paramsSize = sizeof(RgbToYuvFilterParams);
  params.paramsData = &this->rgbToYuvFilterParams;
  params.shaderPath = SHADER(rgb2yuv.comp.glsl.spv);

  this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
  this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
  VkResult ret = this->computeSubGraph->Init();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  PushConstantInfo pushConstantInfo;
  pushConstantInfo.size = params.paramsSize;
  pushConstantInfo.data = params.paramsData;

  PipelineNodeBuffer pipelineNodeInput;
  pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
  pipelineNodeInput.buf.buffer = inputImageInfo[0].storageBuffer;
  pipelineNodeInput.buf.bufferSize = inputImageInfo[0].bufferSize;

  PipelineNodeBuffer pipelineNodeOutput;
  pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
  pipelineNodeOutput.buf.buffer = outputImageInfo[0].storageBuffer;
  pipelineNodeOutput.buf.bufferSize = outputImageInfo[0].bufferSize;

  std::vector<PipelineNodeBuffer> pipelineBuffers;
  pipelineBuffers.push_back(pipelineNodeInput);
  pipelineBuffers.push_back(pipelineNodeOutput);

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
          VK_SHADER_STAGE_COMPUTE_BIT));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
          VK_SHADER_STAGE_COMPUTE_BIT));

  const auto node = std::make_shared<ComputePipelineNode>(
      gpuCtx, "RgbToYuv", params.shaderPath, pushConstantInfo.size,
      descriptorSetLayoutBindings, (inputImageInfo[0].width + 31) / 32,
      (inputImageInfo[0].height + 31) / 32, 1);
  ret = node->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  node->AddComputeElement(
      {.pushConstantInfo = pushConstantInfo, .buffers = pipelineBuffers});

  computeSubGraph->AddComputeGraphNode(node);
  computeGraph->AddSubGraph(this->computeSubGraph);

  return computeGraph->Compute();
}

void RgbToYuvFilter::Destroy() { BasicFilter::Destroy(); }
