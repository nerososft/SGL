//
// Created by neo on 25-4-7.
//

#include "SubComputeGraph.h"

#ifdef OS_OPEN_HARMONY
#include <gpu_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "gpu/VkGPUHelper.h"
#include "log/Log.h"

SubComputeGraph::SubComputeGraph(const std::shared_ptr<VkGPUContext> &gpuCtx) {
    this->gpuCtx = gpuCtx;
}

VkResult SubComputeGraph::Init() {
    VkResult ret = VkGPUHelper::CreateFence(gpuCtx->GetCurrentDevice(), &this->computeFence);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute fence, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    ret = VkGPUHelper::CreateSemaphore(gpuCtx->GetCurrentDevice(), &this->computeDoneSemaphore);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute done semaphore, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    queue = this->gpuCtx->DispatchQueue(VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT);
    if (queue.queue == VK_NULL_HANDLE) {
        Logger() << "Failed to create compute queue, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    ret = VkGPUHelper::AllocateCommandBuffers(gpuCtx->GetCurrentDevice(),
                                              gpuCtx->GetCommandPool(queue.queueFamilyIndex),
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

void SubComputeGraph::AddDependence(const std::shared_ptr<SubComputeGraph> &dependence) {
    this->dependence.push_back(dependence);
}

void SubComputeGraph::AddComputeGraphNode(const std::shared_ptr<IComputeGraphNode> &computeGraphNode) {
    this->computeGraphNodes.push_back(computeGraphNode);
}

VkResult SubComputeGraph::Compute() const {
    vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    vkResetFences(this->gpuCtx->GetCurrentDevice(), 1, &computeFence);

    VkGPUHelper::GPUBeginCommandBuffer(commandBuffer);

    if (!this->computeGraphNodes.empty()) {
        for (const auto &computeGraphNode: this->computeGraphNodes) {
            computeGraphNode->Compute(commandBuffer);
        }
    }

    VkGPUHelper::GPUEndCommandBuffer(commandBuffer);

    std::vector<VkCommandBuffer> submitCommandBuffers;
    submitCommandBuffers.push_back(commandBuffer);

    std::vector<VkSemaphore> submitSignalSemaphores;
    submitSignalSemaphores.push_back(this->computeDoneSemaphore);

    std::vector<VkSemaphore> submitWaitSemaphores;
    submitWaitSemaphores.reserve(this->dependence.size());
    for (const auto &dep: this->dependence) {
        submitWaitSemaphores.push_back(dep->GetComputeDoneSemaphore());
    }

    const VkPipelineStageFlags submitWaitDstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

    std::vector<VkSubmitInfo> submitInfos;
    submitInfos.push_back(VkGPUHelper::BuildSubmitInfo(&submitWaitDstStageMask,
                                                       submitCommandBuffers,
                                                       submitSignalSemaphores,
                                                       submitWaitSemaphores));

    const VkResult ret = VkGPUHelper::GPUQueueSubmit(queue.queue,
                                                     submitInfos,
                                                     this->computeFence);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to submit command buffer, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}

void SubComputeGraph::Destroy() {
    if (computeFence != VK_NULL_HANDLE) {
        vkDestroyFence(gpuCtx->GetCurrentDevice(), computeFence, nullptr);
        computeFence = VK_NULL_HANDLE;
    }

    if (computeDoneSemaphore != VK_NULL_HANDLE) {
        vkDestroySemaphore(gpuCtx->GetCurrentDevice(), computeDoneSemaphore, nullptr);
        computeDoneSemaphore = VK_NULL_HANDLE;
    }
    if (commandBuffer != VK_NULL_HANDLE) {
        std::vector<VkCommandBuffer> freeCommandBuffers;
        freeCommandBuffers.push_back(commandBuffer);
        vkFreeCommandBuffers(gpuCtx->GetCurrentDevice(),
                             gpuCtx->GetCommandPool(queue.queueFamilyIndex),
                             freeCommandBuffers.size(),
                             freeCommandBuffers.data());
        commandBuffer = VK_NULL_HANDLE;
    }
    for (const auto &computeGraphNode: this->computeGraphNodes) {
        computeGraphNode->Destroy();
    }
    computeGraphNodes.clear();

    for (const auto &dep: this->dependence) {
        dep->Destroy();
    }
    dependence.clear();
}
