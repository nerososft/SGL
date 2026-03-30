//
// Created by neo on 2026/1/23.
//

#include "TileBasedVerticalBlurFilter.h"

#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/gpu/compute_graph/ComputePipelineNode.h"
#include "runtime/log/Log.h"

#ifdef OS_OPEN_HARMONY
#include <runtime/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

VkResult TileBasedVerticalBlurFilter::Apply(
    const std::shared_ptr<VkGPUContext> &gpuCtx, int tileIdx,
    const std::vector<FilterImageInfo> &inputImageInfo,
    const std::vector<FilterImageInfo> &outputImageInfo) {
  if (!gpuCtx) {
    Logger() << "TileBasedFilter: invalid GPU context" << std::endl;
    return VK_ERROR_UNKNOWN;
  }

  if (inputImageInfo.size() != 9) {
    Logger() << "TileBasedFilter: need 9 input tile buffers, got "
             << inputImageInfo.size() << std::endl;
    return VK_ERROR_UNKNOWN;
  }

  if (outputImageInfo.empty() || outputImageInfo.size() > 1) {
    Logger() << "TileBasedFilter: need exactly 1 output buffer, got "
             << outputImageInfo.size() << std::endl;
    return VK_ERROR_UNKNOWN;
  }

  Logger() << "TileBasedFilter: tile " << tileIdx
           << ", input tiles:" << inputImageInfo.size()
           << ", output buffers:" << outputImageInfo.size() << std::endl;

  if (computeGraph) {
    computeGraph->Destroy();
  }
  if (computeSubGraph) {
    computeSubGraph->Destroy();
  }

  this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
  this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
  VkResult ret = this->computeSubGraph->Init();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  blurFilterParams.imageSize.currentTileIndex = tileIdx;
  blurFilterParams.imageSize.tileHeight = 256;
  blurFilterParams.imageSize.imageTotalWidth = inputImageInfo[0].width;
  blurFilterParams.imageSize.imageTotalHeight = inputImageInfo[0].height;
  blurFilterParams.imageSize.channels = inputImageInfo[0].channels;
  blurFilterParams.imageSize.bytesPerLine =
      inputImageInfo[0].width * inputImageInfo[0].channels;

  filterParams.paramsSize = sizeof(TiledVerticalBlurFilterParams);
  filterParams.paramsData = &blurFilterParams;
  filterParams.shaderPath = SHADER(tiled_vertical_blur.comp.glsl.spv);

  PushConstantInfo pushConstantInfo;
  pushConstantInfo.size = filterParams.paramsSize;
  pushConstantInfo.data = filterParams.paramsData;

  std::vector<PipelineNodeBuffer> pipelineBuffers;
  for (FilterImageInfo filterImageInfo : inputImageInfo) {
    PipelineNodeBuffer pipelineNodeInput;
    pipelineNodeInput.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
    pipelineNodeInput.buf.buffer = filterImageInfo.storageBuffer;
    pipelineNodeInput.buf.bufferSize = filterImageInfo.bufferSize;
    pipelineBuffers.push_back(pipelineNodeInput);
  }

  for (FilterImageInfo filterImageInfo : outputImageInfo) {
    PipelineNodeBuffer pipelineNodeOutput;
    pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
    pipelineNodeOutput.buf.buffer = filterImageInfo.storageBuffer;
    pipelineNodeOutput.buf.bufferSize = filterImageInfo.bufferSize;
    pipelineBuffers.push_back(pipelineNodeOutput);
  }

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

  descriptorSetLayoutBindings.reserve(pipelineBuffers.size());
  for (int i = 0; i < pipelineBuffers.size(); i++) {
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(
            i, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
            VK_SHADER_STAGE_COMPUTE_BIT));
  }

  uint32_t workGroupX = inputImageInfo[0].width;
  uint32_t workGroupY = (blurFilterParams.imageSize.tileHeight + 31) / 32;

  const auto node = std::make_shared<ComputePipelineNode>(
      gpuCtx, "TileBasedFilter", filterParams.shaderPath, pushConstantInfo.size,
      descriptorSetLayoutBindings, workGroupX, workGroupY, 1);
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

void TileBasedVerticalBlurFilter::Destroy() {}