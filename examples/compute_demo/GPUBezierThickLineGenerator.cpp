//
// Created by neo on 25-6-3.
//

#include "GPUBezierThickLineGenerator.h"

#include "core/gpu/VkGPUHelper.h"
#include "core/gpu/compute_graph/ComputeGraph.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"
#include "core/utils/ImageUtils.h"
#include "core/utils/TimeUtils.h"

GPUBezierThickLineGenerator::~GPUBezierThickLineGenerator() {
  if (this->outputBuffer != nullptr) {
    this->outputBuffer->UnMapBuffer();
    this->outputBuffer->Destroy();
    this->outputBuffer = nullptr;
  }
  if (this->inputBuffer != nullptr) {
    this->inputBuffer->Destroy();
    this->inputBuffer = nullptr;
  }
  if (bezierNode != nullptr) {
    bezierNode->Destroy();
    bezierNode = nullptr;
  }
  if (computeSubGraph != nullptr) {
    computeSubGraph->Destroy();
    computeSubGraph = nullptr;
  }
  if (computeGraph != nullptr) {
    computeGraph->Destroy();
    computeGraph = nullptr;
  }
}

bool GPUBezierThickLineGenerator::InitializeGPUPipeline(
    const BezierParams &bezierParams) {
  this->params = bezierParams;
  std::vector<const char *> extensions = {};
  gpuCtx = std::make_shared<VkGPUContext>(extensions);

  VkResult result = VK_SUCCESS;

  result = gpuCtx->Init();
  if (result != VK_SUCCESS) {
    Logger() << "Failed to initialize GPU context!" << std::endl;
    return false;
  }

  computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
  computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
  result = computeSubGraph->Init();
  if (result != VK_SUCCESS) {
    Logger() << "Failed to initialize sub graph!" << std::endl;
    return false;
  }
  computeGraph->AddSubGraph(computeSubGraph);
  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
          VK_SHADER_STAGE_COMPUTE_BIT));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
          VK_SHADER_STAGE_COMPUTE_BIT));
  bezierNode = std::make_shared<ComputePipelineNode>(
      gpuCtx, "BezierThickLine", SHADER(bezier_thick.comp.glsl.spv),
      sizeof(BezierParams), descriptorSetLayoutBindings,
      (params.bodyPointsNums + 255) / 256, 1, 1);
  result = bezierNode->CreateComputeGraphNode();
  if (result != VK_SUCCESS) {
    Logger() << "Failed to create compute graph node!" << std::endl;
    return false;
  }

  inputBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
  result = inputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                        MAX_LINE_NUMS * sizeof(BezierLine));
  if (result != VK_SUCCESS) {
    Logger() << "Failed to allocate GPU buffer!" << std::endl;
    return false;
  }

  const size_t pointsNums =
      params.bodyPointsNums * 2 + params.assPointsNums + params.headPointsNums;

  outputBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
  result = outputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                         MAX_LINE_NUMS * pointsNums *
                                             sizeof(Point2D));
  if (result != VK_SUCCESS) {
    Logger() << "Failed to allocate GPU buffer!" << std::endl;
    return false;
  }

  std::vector<PipelineNodeBuffer> ppBuffers;
  ppBuffers.push_back({.type = PIPELINE_NODE_BUFFER_STORAGE_READ,
                       .buf = {.bufferSize = inputBuffer->GetBufferSize(),
                               .buffer = inputBuffer->GetBuffer()}});
  ppBuffers.push_back({.type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
                       .buf = {.bufferSize = outputBuffer->GetBufferSize(),
                               .buffer = outputBuffer->GetBuffer()}});

  const PushConstantInfo pushConstantInfo{
      .size = sizeof(BezierParams),
      .data = &params,
  };
  const ComputeElement element = {.pushConstantInfo = pushConstantInfo,
                                  .buffers = ppBuffers,
                                  .customDrawFunc = nullptr};
  bezierNode->AddComputeElement(element);

  result = outputBuffer->MapBuffer();
  if (result != VK_SUCCESS) {
    Logger() << "Failed to map output buffer!" << std::endl;
    return false;
  }

  computeSubGraph->AddComputeGraphNode(bezierNode);
  return true;
}

Point2D *GPUBezierThickLineGenerator::GenerateThickLine(
    const std::vector<BezierLine> &lines) {
  this->params.lineNums = lines.size();
  inputBuffer->UploadData(lines.data(), lines.size() * sizeof(BezierLine));

  const uint64_t time = TimeUtils::GetCurrentMonoMs();
  if (const VkResult ret = computeGraph->Compute(); ret != VK_SUCCESS) {
    Logger() << "Failed to compute graph!" << std::endl;
    return nullptr;
  }
  const uint64_t elapsed = TimeUtils::GetCurrentMonoMs() - time;
  Logger() << "TimeUsage: " << elapsed << "ms" << std::endl;

  return static_cast<Point2D *>(outputBuffer->GetMappedAddr());
}
