//
// Created by neo on 2025/3/11.
//

#include "ComputeGraph.h"

#include <iostream>
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/gpu/VkGPUContext.h"
#include "effect_engine/gpu/VkGPUHelper.h"
#include "effect_engine/log/Log.h"

ComputeGraph::ComputeGraph(const std::shared_ptr<VkGPUContext> &gpuCtx) {
    this->gpuCtx = gpuCtx;
}

void ComputeGraph::AddComputeGraphNode(const std::shared_ptr<IComputeGraphNode> &computeGraphNode) {
    this->computeGraphNodes.push_back(computeGraphNode);
}

VkResult ComputeGraph::Init() {
    VkResult ret = VkGPUHelper::CreateFence(gpuCtx->GetCurrentDevice(), &this->computeFence);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute fence, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    ret = VkGPUHelper::AllocateCommandBuffers(gpuCtx->GetCurrentDevice(),
                                              gpuCtx->GetCommandPool(),
                                              1,
                                              &this->commandBuffer);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to allocate command buffer, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    vkResetFences(gpuCtx->GetCurrentDevice(), 1, &computeFence);
    return ret;
}

VkResult ComputeGraph::Compute() const {
    vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    vkResetFences(gpuCtx->GetCurrentDevice(), 1, &computeFence);


    VkGPUHelper::GPUBeginCommandBuffer(commandBuffer);

    if (!this->computeGraphNodes.empty()) {
        for (const auto &computeGraphNode: this->computeGraphNodes) {
            computeGraphNode->Compute(commandBuffer);
        }
    }

    VkGPUHelper::GPUEndCommandBuffer(commandBuffer);

    std::vector<VkCommandBuffer> submitCommandBuffers;
    submitCommandBuffers.push_back(commandBuffer);

    const std::vector<VkSemaphore> submitSignalSemaphores;
    const std::vector<VkSemaphore> submitWaitSemaphores;

    const VkPipelineStageFlags submitWaitDstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

    std::vector<VkSubmitInfo> submitInfos;
    submitInfos.push_back(VkGPUHelper::BuildSubmitInfo(&submitWaitDstStageMask,
                                                       submitCommandBuffers,
                                                       submitSignalSemaphores,
                                                       submitWaitSemaphores));

    VkResult ret = VkGPUHelper::GPUQueueSubmit(gpuCtx->GetQueue(), submitInfos, computeFence);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to submit command buffer, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    std::vector<VkFence> waitFences;
    waitFences.push_back(computeFence);
    ret = vkWaitForFences(gpuCtx->GetCurrentDevice(),
                          waitFences.size(),
                          waitFences.data(),
                          VK_TRUE,
                          UINT64_MAX);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to wait fence, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }

    ret = vkQueueWaitIdle(gpuCtx->GetQueue());
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to wait idle, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}

void ComputeGraph::Destroy() const {

    if (false) {
        Logger() << "[debug]" << __LINE__ << std::endl;
        vkDestroyFence(gpuCtx->GetCurrentDevice(), computeFence, nullptr);
        Logger() << "[debug]" << __LINE__ << std::endl;

        std::vector<VkCommandBuffer> freeCommandBuffers;
        Logger() << "[debug]" << __LINE__ << std::endl;

        freeCommandBuffers.push_back(commandBuffer);
        Logger() << "[debug]" << __LINE__ << std::endl;

        vkFreeCommandBuffers(gpuCtx->GetCurrentDevice(), gpuCtx->GetCommandPool(), freeCommandBuffers.size(),
            freeCommandBuffers.data());
        Logger() << "[debug]" << __LINE__ << std::endl;
    }
    for (const auto &computeGraphNode: this->computeGraphNodes) {
      //  computeGraphNode->Destroy();
    }
}
