//
// Created by neo on 25-4-13.
//

#include "ImageToBufferCopyNode.h"


#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/gpu/VkGPUHelper.h"
#include "effect_engine/log/Log.h"

ImageToBufferCopyNode::ImageToBufferCopyNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                             const std::string &name,
                                             const ImageToCopyNodeImageInfo srcImage,
                                             const ImageToCopyNodeBufferInfo dstBuffer) {
    this->name = name;
    this->type = COMPUTE_GRAPH_NODE_IMAGE_2_BUF_COPY;
    this->srcImage = srcImage;
    this->dstBuffer = dstBuffer;
    this->gpuCtx = gpuCtx;
}

VkResult ImageToBufferCopyNode::CreateComputeGraphNode() {
    if (gpuCtx == nullptr) {
        Logger() << "gpuCtx is null" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return VK_SUCCESS;
}

void ImageToBufferCopyNode::Compute(const VkCommandBuffer commandBuffer) {
    Logger() << "Executing Compute Node: " << name << std::endl;
    if (!this->dependencies.empty()) {
        for (const auto &dependence: this->dependencies) {
            Logger() << "Node: " << name << " Depend On:" << dependence->GetName() << std::endl;
            dependence->Compute(commandBuffer);
        }
    }

    std::vector<VkImageMemoryBarrier> srcImageMemoryBarriers;
    srcImageMemoryBarriers.push_back(VkGPUHelper::BuildImageMemoryBarrier(VK_ACCESS_SHADER_WRITE_BIT,
                                                                          VK_ACCESS_TRANSFER_READ_BIT,
                                                                          srcImage.image,
                                                                          VK_IMAGE_LAYOUT_UNDEFINED,
                                                                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
    VkGPUHelper::GPUCmdPipelineImageMemBarrier(commandBuffer,
                                               VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                                               VK_PIPELINE_STAGE_TRANSFER_BIT,
                                               0,
                                               srcImageMemoryBarriers);
    std::vector<VkBufferImageCopy> copyRegions;
    VkBufferImageCopy copyRegion;
    copyRegion.bufferOffset = 0;
    copyRegion.bufferRowLength = this->srcImage.rowLength;
    copyRegion.bufferImageHeight = this->srcImage.height;
    copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.imageSubresource.layerCount = 1;
    copyRegion.imageSubresource.mipLevel = 0;
    copyRegion.imageSubresource.baseArrayLayer = 0;
    copyRegion.imageOffset = {.x = 0, .y = 0, .z = 0};
    copyRegion.imageExtent = {.width = this->srcImage.width, .height = this->srcImage.height, .depth = 1};
    copyRegions.push_back(copyRegion);

    vkCmdCopyImageToBuffer(commandBuffer,
                           this->srcImage.image,
                           this->srcImage.imageLayout,
                           this->dstBuffer.buffer,
                           copyRegions.size(),
                           copyRegions.data());

    std::vector<VkBufferMemoryBarrier> dtsBufferMemoryBarriers;
    dtsBufferMemoryBarriers.push_back(VkGPUHelper::BuildBufferMemoryBarrier(VK_ACCESS_SHADER_WRITE_BIT,
                                                                            VK_ACCESS_TRANSFER_READ_BIT,
                                                                            dstBuffer.buffer,
                                                                            dstBuffer.bufferSize));
    VkGPUHelper::GPUCmdPipelineBufferMemBarrier(commandBuffer,
                                                VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                0,
                                                dtsBufferMemoryBarriers);
}

void ImageToBufferCopyNode::Destroy() {
    IComputeGraphNode::Destroy();
}
