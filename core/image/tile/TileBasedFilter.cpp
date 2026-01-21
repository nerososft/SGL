//
// Created by neo on 2026/1/20.
//

#include "TileBasedFilter.h"

#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/gpu/compute_graph/ComputeGraph.h"
#include "runtime/gpu/compute_graph/ComputePipelineNode.h"
#include "runtime/log/Log.h"

#include <vulkan/vk_enum_string_helper.h>

VkResult
TileBasedFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx, int tileIdx,
                       const std::vector<FilterImageInfo> &inputImageInfo,
                       const std::vector<FilterImageInfo> &outputImageInfo) {
  Logger() << "TileBasedFilter: tile " << tileIdx
           << ", buffers:" << inputImageInfo.size() << std::endl;
  this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
  this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
  VkResult ret = this->computeSubGraph->Init();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  tiledFilterParams.currentTileIndex = tileIdx;
  tiledFilterParams.tileHeight = 256;
  tiledFilterParams.imageTotalWidth = inputImageInfo[0].width;
  tiledFilterParams.imageTotalHeight = inputImageInfo[0].height;
  tiledFilterParams.channels = inputImageInfo[0].channels;
  tiledFilterParams.bytesPerLine =
      inputImageInfo[0].width * inputImageInfo[0].channels;

  filterParams.paramsSize = sizeof(TileBasedFilterParam);
  filterParams.paramsData = &tiledFilterParams;
  filterParams.shaderPath = SHADER(tiled_gray.comp.glsl.spv);

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

  uint32_t workGroupX = outputImageInfo[0].width + 31 / 32;
  uint32_t workGroupY = 8;

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

void TileBasedFilter::Destroy() {}