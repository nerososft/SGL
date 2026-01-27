//
// Created by neo on 2026/1/22.
//

#include "TileBasedGaussianBlurFilter.h"

#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/gpu/compute_graph/ComputePipelineNode.h"
#include "runtime/log/Log.h"

#ifdef OS_OPEN_HARMONY
#include <runtime/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

std::shared_ptr<IComputeGraphNode>
TileBasedGaussianBlurFilter::CreateTileBasedBlurNode(
    const std::vector<FilterImageInfo> &inputImageInfo,
    const std::string &shaderPath, VkBuffer buffer, VkDeviceSize bufferSize) {
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

  PipelineNodeBuffer pipelineNodeOutput;
  pipelineNodeOutput.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE;
  pipelineNodeOutput.buf.buffer = buffer;
  pipelineNodeOutput.buf.bufferSize = bufferSize;
  pipelineBuffers.push_back(pipelineNodeOutput);

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

  descriptorSetLayoutBindings.reserve(pipelineBuffers.size());
  for (int i = 0; i < pipelineBuffers.size(); i++) {
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(
            i, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
            VK_SHADER_STAGE_COMPUTE_BIT));
  }

  uint32_t workGroupX = inputImageInfo[0].width + 31 / 32;
  uint32_t workGroupY = 8;

  const auto node = std::make_shared<ComputePipelineNode>(
      gpuCtx, "TileBasedFilter", shaderPath, pushConstantInfo.size,
      descriptorSetLayoutBindings, workGroupX, workGroupY, 1);
  VkResult ret = node->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return nullptr;
  }

  node->AddComputeElement(
      {.pushConstantInfo = pushConstantInfo, .buffers = pipelineBuffers});
  return node;
}

VkResult TileBasedGaussianBlurFilter::Apply(
    const std::shared_ptr<VkGPUContext> &gpuCtx, const int tileIdx,
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

  this->gpuCtx = gpuCtx;
  this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
  this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
  VkResult ret = this->computeSubGraph->Init();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute graph, err =" << string_VkResult(ret)
             << std::endl;
    return ret;
  }

  gaussianFilterParams.imageSize.currentTileIndex = tileIdx;
  gaussianFilterParams.imageSize.tileHeight = 256;
  gaussianFilterParams.imageSize.imageTotalWidth = inputImageInfo[0].width;
  gaussianFilterParams.imageSize.imageTotalHeight = inputImageInfo[0].height;
  gaussianFilterParams.imageSize.channels = inputImageInfo[0].channels;
  gaussianFilterParams.imageSize.bytesPerLine =
      inputImageInfo[0].width * inputImageInfo[0].channels;
  filterParams.paramsSize = sizeof(GaussianBlurFilterParams);
  filterParams.paramsData = &gaussianFilterParams;
  if (vBlurOutputBlurBuffer == nullptr) {
    vBlurOutputBlurBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    ret = vBlurOutputBlurBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                                 outputImageInfo[0].bufferSize);
    if (ret != VK_SUCCESS) {
      Logger() << "Failed to allocate blur buffer memory, err ="
               << string_VkResult(ret) << std::endl;
      return ret;
    }
  }

  const std::shared_ptr<IComputeGraphNode> vBlurNode = CreateTileBasedBlurNode(
      inputImageInfo, SHADER(tiled_vertical_blur.comp.glsl.spv),
      vBlurOutputBlurBuffer->GetBuffer(),
      vBlurOutputBlurBuffer->GetBufferSize());

  this->hBlurImageInfo = inputImageInfo;
  hBlurImageInfo[4].storageBuffer = vBlurOutputBlurBuffer->GetBuffer();
  hBlurImageInfo[4].bufferSize = outputImageInfo[0].bufferSize;
  const std::shared_ptr<IComputeGraphNode> hBlurNode = CreateTileBasedBlurNode(
      hBlurImageInfo, SHADER(tiled_horizontal_blur.comp.glsl.spv),
      outputImageInfo[0].storageBuffer, outputImageInfo[0].bufferSize);

  hBlurNode->AddDependenceNode(vBlurNode);
  this->computeSubGraph->AddComputeGraphNode(hBlurNode);
  this->computeGraph->AddSubGraph(computeSubGraph);

  return computeGraph->Compute();
}

void TileBasedGaussianBlurFilter::Destroy() {}
