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

  uint32_t workGroupX = targetWidth + 31 / 32;
  uint32_t workGroupY = targetHeight + 31 / 32;

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
TileBasedFastGaussianBlurFilter::CreateVBlurNode(
    VkBuffer inputBuffer, VkDeviceSize inputBufferSize, VkBuffer outputBuffer,
    VkDeviceSize outputBufferSize) {
  return nullptr;
}
std::shared_ptr<IComputeGraphNode>
TileBasedFastGaussianBlurFilter::CreateTileBasedHBlurNode(
    const std::vector<FilterImageInfo> &inputImageInfo) {
  return nullptr;
}
std::shared_ptr<IComputeGraphNode>
TileBasedFastGaussianBlurFilter::CreateTileBasedScaleUpNode(
    const std::vector<FilterImageInfo> &inputImageInfo, uint32_t targetWidth,
    uint32_t targetHeight) {
  return nullptr;
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
    Logger() << "TileBasedFilter: need 9 input tile buffers, got " << inputImageInfo.size() << std::endl;
    return VK_ERROR_UNKNOWN;
  }

  if (outputImageInfo.empty() || outputImageInfo.size() > 1) {
    Logger() << "TileBasedFilter: need exactly 1 output buffer, got " << outputImageInfo.size() << std::endl;
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

  scaleDownBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
  const VkDeviceSize scaleDownBufferSize = targetWidth * targetHeight * 4;
  ret = scaleDownBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                         scaleDownBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to allocate scale buffer memory, err ="
             << string_VkResult(ret) << std::endl;
    return ret;
  }

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
  const std::shared_ptr<IComputeGraphNode> scaleDownNode =
      CreateTileBasedScaleDownNode(inputImageInfo, targetWidth, targetHeight);

  // this->blurFilterParams.imageSize.width = targetWidth;
  // this->blurFilterParams.imageSize.height = targetHeight;
  // this->blurFilterParams.imageSize.channels = 4;
  // this->blurFilterParams.imageSize.bytesPerLine = targetWidth * 4;
  // this->blurFilterParams.radius = newRadius;
  // scaleDownBlurBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
  // ret = scaleDownBlurBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
  //                                            scaleDownBufferSize);
  // if (ret != VK_SUCCESS) {
  //   Logger() << "Failed to allocate blur buffer memory, err ="
  //            << string_VkResult(ret) << std::endl;
  //   return ret;
  // }
  //
  // const std::shared_ptr<IComputeGraphNode> vBlurNode =
  //     CreateVBlurNode(gpuCtx, scaleDownBuffer->GetBuffer(),
  //     scaleDownBufferSize,
  //                     scaleDownBlurBuffer->GetBuffer(), scaleDownBufferSize);
  // const std::shared_ptr<IComputeGraphNode> hBlurNode = CreateHBlurNode(
  //     gpuCtx, scaleDownBlurBuffer->GetBuffer(), scaleDownBufferSize,
  //     scaleDownBuffer->GetBuffer(), scaleDownBufferSize);
  //
  // scaleUpParams.imageSize.width = targetWidth;
  // scaleUpParams.imageSize.height = targetHeight;
  // scaleUpParams.imageSize.channels = 4;
  // scaleUpParams.imageSize.bytesPerLine = targetWidth * 4;
  // scaleUpParams.targetWidth = inputImageInfo[0].width;
  // scaleUpParams.targetHeight = inputImageInfo[0].height;
  // const std::shared_ptr<IComputeGraphNode> scaleUpNode = CreateScaleUpNode(
  //     gpuCtx, scaleDownBuffer->GetBuffer(), scaleDownBufferSize,
  //     outputImageInfo[0].storageBuffer, outputImageInfo[0].bufferSize,
  //     outputImageInfo[0].width, outputImageInfo[0].height);
  //
  // vBlurNode->AddDependenceNode(scaleDownNode);
  // hBlurNode->AddDependenceNode(vBlurNode);
  // scaleUpNode->AddDependenceNode(hBlurNode);
  // this->computeSubGraph->AddComputeGraphNode(scaleUpNode);

  this->computeSubGraph->AddComputeGraphNode(scaleDownNode);
  this->computeGraph->AddSubGraph(computeSubGraph);

  return computeGraph->Compute();
}

void TileBasedFastGaussianBlurFilter::Destroy() {}
