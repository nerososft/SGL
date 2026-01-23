//
// Created by neo on 2026/1/22.
//

#include "TileBasedFastGaussianBlurFilter.h"

#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/gpu/compute_graph/ComputePipelineNode.h"
#include "runtime/log/Log.h"

#include <vulkan/vk_enum_string_helper.h>

std::shared_ptr<IComputeGraphNode>
TileBasedFastGaussianBlurFilter::CreateTileBasedScaleDownNode(
    const std::vector<FilterImageInfo> &inputImageInfo, uint32_t targetWidth,
    uint32_t targetHeight) {
  PushConstantInfo pushConstantInfo;
  pushConstantInfo.size = scaleDownParams.paramsSize;
  pushConstantInfo.data = scaleDownParams.paramsData;

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
  pipelineNodeOutput.buf.buffer = scaleDownBuffer->GetBuffer();
  pipelineNodeOutput.buf.bufferSize = scaleDownBuffer->GetBufferSize();
  pipelineBuffers.push_back(pipelineNodeOutput);

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

  descriptorSetLayoutBindings.reserve(pipelineBuffers.size());
  for (int i = 0; i < pipelineBuffers.size(); i++) {
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(
            i, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
            VK_SHADER_STAGE_COMPUTE_BIT));
  }

  uint32_t workGroupX = (targetWidth + 31) / 32;
  uint32_t workGroupY = (targetHeight + 31) / 32;

  const auto node = std::make_shared<ComputePipelineNode>(
      gpuCtx, "TileBasedScaleDownFilter", scaleDownParams.shaderPath,
      pushConstantInfo.size, descriptorSetLayoutBindings, workGroupX,
      workGroupY, 1);
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

std::shared_ptr<IComputeGraphNode>
TileBasedFastGaussianBlurFilter::CreateTileBasedVBlurNode(
    const std::vector<FilterImageInfo> &inputImageInfo, uint32_t targetWidth,
    uint32_t targetHeight, const std::string &shaderPath) {
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
  pipelineNodeOutput.buf.buffer = scaleDownBlurBuffer->GetBuffer();
  pipelineNodeOutput.buf.bufferSize = scaleDownBlurBuffer->GetBufferSize();
  pipelineBuffers.push_back(pipelineNodeOutput);

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

  descriptorSetLayoutBindings.reserve(pipelineBuffers.size());
  for (int i = 0; i < pipelineBuffers.size(); i++) {
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(
            i, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
            VK_SHADER_STAGE_COMPUTE_BIT));
  }

  uint32_t workGroupX = (targetWidth + 31) / 32;
  uint32_t workGroupY = (targetHeight + 31) / 32;

  const auto node = std::make_shared<ComputePipelineNode>(
      gpuCtx, "TileBasedVFilter", shaderPath, pushConstantInfo.size,
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

std::shared_ptr<IComputeGraphNode>
TileBasedFastGaussianBlurFilter::CreateTileBasedHBlurNode(
    const std::vector<FilterImageInfo> &inputImageInfo, uint32_t targetWidth,
    uint32_t targetHeight, const std::string &shaderPath) {
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
  pipelineNodeOutput.buf.buffer = scaleDownBuffer->GetBuffer();
  pipelineNodeOutput.buf.bufferSize = scaleDownBuffer->GetBufferSize();
  pipelineBuffers.push_back(pipelineNodeOutput);

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

  descriptorSetLayoutBindings.reserve(pipelineBuffers.size());
  for (int i = 0; i < pipelineBuffers.size(); i++) {
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(
            i, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
            VK_SHADER_STAGE_COMPUTE_BIT));
  }

  uint32_t workGroupX = (targetWidth + 31) / 32;
  uint32_t workGroupY = (targetHeight + 31) / 32;

  const auto node = std::make_shared<ComputePipelineNode>(
      gpuCtx, "TileBasedHFilter", shaderPath, pushConstantInfo.size,
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

std::shared_ptr<IComputeGraphNode>
TileBasedFastGaussianBlurFilter::CreateTileBasedScaleUpNode(
    const std::vector<FilterImageInfo> &inputImageInfo, uint32_t targetWidth,
    uint32_t targetHeight, VkBuffer outputBuffer,
    VkDeviceSize outputBufferSize) {
  PushConstantInfo pushConstantInfo;
  pushConstantInfo.size = scaleUpParams.paramsSize;
  pushConstantInfo.data = scaleUpParams.paramsData;

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
  pipelineNodeOutput.buf.buffer = outputBuffer;
  pipelineNodeOutput.buf.bufferSize = outputBufferSize;
  pipelineBuffers.push_back(pipelineNodeOutput);

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

  descriptorSetLayoutBindings.reserve(pipelineBuffers.size());
  for (int i = 0; i < pipelineBuffers.size(); i++) {
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(
            i, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
            VK_SHADER_STAGE_COMPUTE_BIT));
  }

  uint32_t workGroupX = (targetWidth + 31) / 32;
  uint32_t workGroupY = (targetHeight + 31) / 32;

  const auto node = std::make_shared<ComputePipelineNode>(
      gpuCtx, "TileBasedScaleUpFilter", scaleUpParams.shaderPath,
      pushConstantInfo.size, descriptorSetLayoutBindings, workGroupX,
      workGroupY, 1);
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

VkResult TileBasedFastGaussianBlurFilter::Apply(
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

  uint32_t ratio = this->gaussianFilterParams.radius / 3;
  ratio = ratio > 4 ? 4 : ratio;
  ratio = ratio < 2 ? 2 : ratio;
  Logger() << "Downsampling ratio: " << ratio << std::endl;
  const uint32_t targetWidth = inputImageInfo[0].width / ratio;
  const uint32_t targetHeight = 256 / ratio;
  const int newRadius = this->gaussianFilterParams.radius / ratio;

  scaleDownFilterParams.imageSize.imageTotalWidth = inputImageInfo[0].width;
  scaleDownFilterParams.imageSize.imageTotalHeight = inputImageInfo[0].height;
  scaleDownFilterParams.imageSize.channels = 4;
  scaleDownFilterParams.imageSize.bytesPerLine = inputImageInfo[0].width * 4;
  scaleDownFilterParams.imageSize.tileHeight = 256;
  scaleDownFilterParams.imageSize.currentTileIndex = tileIdx;
  scaleDownFilterParams.targetWidth = targetWidth;
  scaleDownFilterParams.targetHeight = targetHeight;
  scaleDownFilterParams.interpType = 1;
  scaleDownParams.paramsData = &scaleDownFilterParams;
  scaleDownParams.paramsSize = sizeof(TiledScaleFilterParams);
  scaleDownParams.shaderPath = SHADER(tiled_scale.comp.glsl.spv);
  scaleDownBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
  const VkDeviceSize scaleDownBufferSize = targetWidth * targetHeight * 4;
  ret = scaleDownBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                         scaleDownBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to allocate scale buffer memory, err ="
             << string_VkResult(ret) << std::endl;
    return ret;
  }

  const std::shared_ptr<IComputeGraphNode> scaleDownNode =
      CreateTileBasedScaleDownNode(inputImageInfo, targetWidth, targetHeight);

  gaussianFilterParams.imageSize.imageTotalWidth = inputImageInfo[0].width;
  gaussianFilterParams.imageSize.imageTotalHeight = inputImageInfo[0].height;
  gaussianFilterParams.imageSize.channels = 4;
  gaussianFilterParams.imageSize.bytesPerLine = inputImageInfo[0].width * 4;
  gaussianFilterParams.imageSize.tileHeight = 256;
  gaussianFilterParams.imageSize.currentTileIndex = tileIdx;
  gaussianFilterParams.radius = newRadius;
  filterParams.paramsData = &gaussianFilterParams;
  filterParams.paramsSize = sizeof(FastGaussianBlurFilterParams);
  scaleDownBlurBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
  ret = scaleDownBlurBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                             scaleDownBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to allocate blur buffer memory, err ="
             << string_VkResult(ret) << std::endl;
    return ret;
  }

  const std::shared_ptr<IComputeGraphNode> vBlurNode =
      CreateTileBasedVBlurNode(inputImageInfo, targetWidth, targetHeight,
                               SHADER(tiled_vertical_blur.comp.glsl.spv));

  const std::shared_ptr<IComputeGraphNode> hBlurNode =
      CreateTileBasedHBlurNode(inputImageInfo, targetWidth, targetHeight,
                               SHADER(tiled_horizontal_blur.comp.glsl.spv));

  scaleUpFilterParams.imageSize.imageTotalWidth = inputImageInfo[0].width;
  scaleUpFilterParams.imageSize.imageTotalHeight = inputImageInfo[0].height;
  scaleUpFilterParams.imageSize.channels = 4;
  scaleUpFilterParams.imageSize.bytesPerLine = inputImageInfo[0].width * 4;
  scaleUpFilterParams.imageSize.tileHeight = 256;
  scaleUpFilterParams.imageSize.currentTileIndex = tileIdx;
  scaleUpFilterParams.targetWidth = inputImageInfo[0].width;
  scaleUpFilterParams.targetHeight = inputImageInfo[0].height;
  scaleUpFilterParams.interpType = 1;
  scaleUpParams.paramsData = &scaleUpFilterParams;
  scaleUpParams.paramsSize = sizeof(TiledScaleFilterParams);
  scaleUpParams.shaderPath = SHADER(tiled_scale.comp.glsl.spv);
  const std::shared_ptr<IComputeGraphNode> scaleUpNode =
      CreateTileBasedScaleUpNode(
          inputImageInfo, inputImageInfo[0].width, inputImageInfo[0].height,
          outputImageInfo[0].storageBuffer, outputImageInfo[0].bufferSize);

  vBlurNode->AddDependenceNode(scaleDownNode);
  hBlurNode->AddDependenceNode(vBlurNode);
  scaleUpNode->AddDependenceNode(hBlurNode);
  this->computeSubGraph->AddComputeGraphNode(scaleUpNode);
  this->computeGraph->AddSubGraph(computeSubGraph);

  return computeGraph->Compute();
}

void TileBasedFastGaussianBlurFilter::Destroy() {}
