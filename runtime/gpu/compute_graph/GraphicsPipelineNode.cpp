//
// Created by neo on 2025/3/25.
//

#include "GraphicsPipelineNode.h"

#ifdef OS_OPEN_HARMONY
#include <gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "gpu/VkGPUGraphicsPipeline.h"
#include "gpu/VkGPUHelper.h"
#include "log/Log.h"

GraphicsPipelineNode::GraphicsPipelineNode(
    const std::shared_ptr<VkGPUContext> &gpuCtx, const std::string &name,
    const std::shared_ptr<VkGPURenderPass> &renderPass,
    const std::string &vertexShaderPath, const std::string &fragmentShaderPath,
    const uint32_t pushConstantSize,
    const std::vector<VkDescriptorSetLayoutBinding>
        &descriptorSetLayoutBindings,
    const std::vector<VkVertexInputBindingDescription>
        &vertexInputBindingDescriptions,
    const std::vector<VkVertexInputAttributeDescription>
        &vertexInputAttributeDescriptions,
    const float width, const float height) {
  this->gpuCtx = gpuCtx;
  this->name = name;
  this->type = COMPUTE_GRAPH_NODE_GRAPHICS;
  this->renderPass = renderPass;
  this->vertexShaderPath = vertexShaderPath;
  this->fragmentShaderPath = fragmentShaderPath;
  this->pushConstantSize = pushConstantSize;
  this->descriptorSetLayoutBindings = descriptorSetLayoutBindings;
  this->vertexInputBindingDescriptions = vertexInputBindingDescriptions;
  this->vertexInputAttributeDescriptions = vertexInputAttributeDescriptions;
  this->width = width;
  this->height = height;
}

std::shared_ptr<VkGPUDescriptorSet> GraphicsPipelineNode::CreateDescriptorSet(
    const GraphicsElement &graphicsElement) const {
  auto descriptorSet = std::make_shared<VkGPUDescriptorSet>(
      gpuCtx->GetCurrentDevice(), graphicsPipeline->GetPipelineLayout(),
      graphicsPipeline->GetDescriptorSetLayout());
  const VkResult ret =
      descriptorSet->AllocateDescriptorSets(gpuCtx->GetDescriptorPool());
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to allocate descriptor sets, err ="
             << string_VkResult(ret) << std::endl;
    return nullptr;
  }

  std::vector<PipelineDescriptorInfo> pipelineDescriptorInfos;
  for (const auto &buffer : graphicsElement.buffers) {
    if (buffer.type == PIPELINE_NODE_BUFFER_VERTEX ||
        buffer.type == PIPELINE_NODE_BUFFER_INDEX) {
      continue;
    }
    if (buffer.type == PIPELINE_NODE_BUFFER_STORAGE_READ ||
        buffer.type == PIPELINE_NODE_BUFFER_STORAGE_WRITE ||
        buffer.type == PIPELINE_NODE_BUFFER_UNIFORM) {
      PipelineDescriptorInfo info;
      VkDescriptorBufferInfo bufferInfo = {};
      bufferInfo.offset = 0;
      bufferInfo.range = buffer.buf.bufferSize;
      bufferInfo.buffer = buffer.buf.buffer;
      info.bufferInfo = bufferInfo;
      pipelineDescriptorInfos.push_back(info);
    } else if (buffer.type == PIPELINE_NODE_SAMPLER) {
      PipelineDescriptorInfo info;
      VkDescriptorImageInfo imageInfo = {};
      imageInfo.imageView = buffer.sampler.imageView;
      imageInfo.imageLayout = buffer.sampler.imageLayout;
      imageInfo.sampler = buffer.sampler.sampler;
      info.imageInfo = imageInfo;
      pipelineDescriptorInfos.push_back(info);
    } else {
      Logger() << "unsupported type" << std::endl;
    }
  }

  for (uint32_t i = 0; i < pipelineDescriptorInfos.size(); ++i) {
    Logger() << Logger::DEBUG << "Descriptor(" << i << "):"
             << string_VkDescriptorType(
                    this->descriptorSetLayoutBindings[i].descriptorType)
             << std::endl;
    if (this->descriptorSetLayoutBindings[i].descriptorType ==
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
      descriptorSet->AddUniformBufferDescriptorSet(
          i, pipelineDescriptorInfos.at(i).bufferInfo);
    } else if (this->descriptorSetLayoutBindings[i].descriptorType ==
               VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
      descriptorSet->AddStorageBufferDescriptorSet(
          i, pipelineDescriptorInfos.at(i).bufferInfo);
    } else if (this->descriptorSetLayoutBindings[i].descriptorType ==
               VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
      descriptorSet->AddSamplerDescriptorSet(
          i, pipelineDescriptorInfos.at(i).imageInfo);
    } else {
      Logger() << "Unsupported descriptor type " << std::endl;
      return nullptr;
    }
  }
  descriptorSet->UpdateDescriptorSets();
  return descriptorSet;
}

void GraphicsPipelineNode::AddGraphicsElement(
    const GraphicsElement &graphicsElement) {
  this->graphicsElements.push_back(graphicsElement);
  pipelineDescriptorSets.push_back(CreateDescriptorSet(graphicsElement));
}

VkResult GraphicsPipelineNode::CreateComputeGraphNode() {
  if (gpuCtx == nullptr) {
    Logger() << "gpuCtx is null" << std::endl;
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  for (auto &layoutBinding : descriptorSetLayoutBindings) {
    Logger() << Logger::DEBUG << "Binding(" << layoutBinding.binding
             << "):" << string_VkDescriptorType(layoutBinding.descriptorType)
             << std::endl;
  }

  std::vector<VkPushConstantRange> pushConstantRanges;
  VkPushConstantRange pushConstantRange;
  pushConstantRange.offset = 0;
  pushConstantRange.size = this->pushConstantSize;
  pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
  pushConstantRanges.push_back(pushConstantRange);

  graphicsPipeline = std::make_shared<VkGPUGraphicsPipeline>(
      vertexShaderPath, fragmentShaderPath, width, height,
      descriptorSetLayoutBindings, pushConstantRanges,
      vertexInputBindingDescriptions, vertexInputAttributeDescriptions);
  const VkResult ret = graphicsPipeline->CreateGraphicsPipeline(
      gpuCtx->GetCurrentDevice(), gpuCtx->GetPipelineCache(),
      renderPass->GetRenderPass());
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create compute pipeline, err ="
             << string_VkResult(ret) << std::endl;
    return ret;
  }

  return ret;
}

void GraphicsPipelineNode::Compute(const VkCommandBuffer commandBuffer) {
  if (!this->dependencies.empty()) {
    for (const auto &dependence : this->dependencies) {
      Logger() << Logger::DEBUG << "Node: " << name
               << " Depend On:" << dependence->GetName() << std::endl;
      dependence->Compute(commandBuffer);
    }
  }
  Logger() << Logger::DEBUG << "Executing Compute Node: " << name << std::endl;
  graphicsPipeline->GPUCmdBindPipeline(commandBuffer);
  for (size_t i = 0; i < graphicsElements.size(); ++i) {
    pipelineDescriptorSets[i]->GPUCmdBindDescriptorSets(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS);
    VkGPUHelper::GPUCmdPushConstant(commandBuffer,
                                    graphicsPipeline->GetPipelineLayout(),
                                    VK_SHADER_STAGE_ALL_GRAPHICS, 0,
                                    graphicsElements[i].pushConstantInfo.size,
                                    graphicsElements[i].pushConstantInfo.data);

    std::vector<VkBuffer> bindVertexBuffers;
    std::vector<VkDeviceSize> bindVertexOffsets;
    std::vector<VkBuffer> bindIndexBuffers;
    int32_t indexCount = 0;
    for (const auto &buffer : graphicsElements[i].buffers) {
      if (buffer.type == PIPELINE_NODE_BUFFER_VERTEX) {
        if (buffer.buf.buffer == VK_NULL_HANDLE) {
          Logger() << " Buffer is null" << std::endl;
          return;
        }
        bindVertexBuffers.push_back(buffer.buf.buffer);
        bindVertexOffsets.push_back(0);
      }
      if (buffer.type == PIPELINE_NODE_BUFFER_INDEX) {
        bindIndexBuffers.push_back(buffer.buf.buffer);
        indexCount = buffer.buf.bufferSize / sizeof(uint32_t);
      }
      if (buffer.type == PIPELINE_NODE_SAMPLER) {
        std::vector<VkBufferImageCopy> regions;
        VkBufferImageCopy region;
        region.bufferOffset = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent.depth = 1;
        region.imageExtent.width = static_cast<uint32_t>(width);
        region.imageExtent.height = static_cast<uint32_t>(height);
        region.bufferRowLength = static_cast<uint32_t>(width);
        region.bufferImageHeight = static_cast<uint32_t>(height);
        regions.push_back(region);

        std::vector<VkMemoryBarrier> memoryBarriers;
        memoryBarriers.push_back(VkGPUHelper::BuildMemoryBarrier(
            VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT));
        VkGPUHelper::GPUCmdPipelineMemBarrier(
            commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT, 0, memoryBarriers);
        static bool uploaded = false; // FIXME: tempory change
        if (!uploaded) {
          vkCmdCopyBufferToImage(
              commandBuffer, buffer.sampler.imageBuffer, buffer.sampler.image,
              buffer.sampler.imageLayout, regions.size(), regions.data());
          uploaded = true;
        }

        memoryBarriers.clear();
        memoryBarriers.push_back(VkGPUHelper::BuildMemoryBarrier(
            VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT));

        VkGPUHelper::GPUCmdPipelineMemBarrier(
            commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, memoryBarriers);
      }
    }
    if (!bindVertexBuffers.empty()) {
      vkCmdBindVertexBuffers(commandBuffer, 0, bindVertexBuffers.size(),
                             bindVertexBuffers.data(),
                             bindVertexOffsets.data());
    }
    for (const auto &buffer : bindIndexBuffers) {
      vkCmdBindIndexBuffer(commandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
    }
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);

    if (graphicsElements[i].customDrawFunc != nullptr) {
      graphicsElements[i].customDrawFunc(commandBuffer);
    }
  }
}

void GraphicsPipelineNode::Destroy() {
  if (graphicsPipeline != nullptr) {
    graphicsPipeline->Destroy();
    graphicsPipeline = nullptr;
  }
  for (const auto &pipelineDescriptorSet : pipelineDescriptorSets) {
    pipelineDescriptorSet->Destroy();
  }
  pipelineDescriptorSets.clear();
  IComputeGraphNode::Destroy();
}
