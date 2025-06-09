//
// Created by neo on 2025/3/11.
//

#include "BufferCopyNode.h"

#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "gpu/VkGPUHelper.h"
#include "log/Log.h"

BufferCopyNode::BufferCopyNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                               const std::string &name,
                               const BufferCopyNodeBufferInfo srcBuffer,
                               const BufferCopyNodeBufferInfo dstBuffer) {
    this->name = name;
    this->type = COMPUTE_GRAPH_NODE_BUF_2_BUF_COPY;
    this->srcBuffer = srcBuffer;
    this->dstBuffer = dstBuffer;
    this->gpuCtx = gpuCtx;
}

VkResult BufferCopyNode::CreateComputeGraphNode() {
    if (gpuCtx == nullptr) {
        Logger() << "gpuCtx is null" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return VK_SUCCESS;
}

void BufferCopyNode::Compute(const VkCommandBuffer commandBuffer) {
    Logger() << "Executing Compute Node: " << name << std::endl;
    if (!this->dependencies.empty()) {
        for (const auto &dependence: this->dependencies) {
            Logger() << "Node: " << name << " Depend On:" << dependence->GetName() << std::endl;
            dependence->Compute(commandBuffer);
        }
    }

    std::vector<VkBufferMemoryBarrier> srcBufferMemoryBarriers;
    srcBufferMemoryBarriers.push_back(VkGPUHelper::BuildBufferMemoryBarrier(VK_ACCESS_SHADER_WRITE_BIT,
                                                                            VK_ACCESS_TRANSFER_READ_BIT,
                                                                            srcBuffer.buffer,
                                                                            srcBuffer.bufferSize));
    VkGPUHelper::GPUCmdPipelineBufferMemBarrier(commandBuffer,
                                                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                0,
                                                srcBufferMemoryBarriers);
    std::vector<VkBufferCopy> copyRegions;
    VkBufferCopy copyRegion;
    copyRegion.size = srcBuffer.bufferSize;
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegions.push_back(copyRegion);
    vkCmdCopyBuffer(commandBuffer,
                    this->srcBuffer.buffer,
                    this->dstBuffer.buffer,
                    copyRegions.size(),
                    copyRegions.data());

    std::vector<VkBufferMemoryBarrier> dtsBufferMemoryBarriers;
    dtsBufferMemoryBarriers.push_back(VkGPUHelper::BuildBufferMemoryBarrier(VK_ACCESS_SHADER_WRITE_BIT,
                                                                            VK_ACCESS_TRANSFER_READ_BIT,
                                                                            dstBuffer.buffer,
                                                                            dstBuffer.bufferSize));
    VkGPUHelper::GPUCmdPipelineBufferMemBarrier(commandBuffer,
                                                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                0,
                                                dtsBufferMemoryBarriers);
}

void BufferCopyNode::Destroy() {
    IComputeGraphNode::Destroy();
}
