//
// Created by neo on 2025/3/25.
//

#include "VkGPURenderPass.h"

#include "VkGPUHelper.h"
#include "effect_engine/log/Log.h"

VkGPURenderPass::VkGPURenderPass(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                 const std::vector<VkAttachmentDescription> &attachments,
                                 const std::vector<VkSubpassDependency> &dependencies,
                                 const std::vector<VkSubpassDescription> &subPasses,
                                 const VkRect2D renderArea,
                                 const std::vector<VkClearValue> &clearValues) {
    this->gpuCtx = gpuCtx;
    this->attachments = attachments;
    this->dependencies = dependencies;
    this->subPasses = subPasses;
    this->renderArea = renderArea;
    this->clearValues = clearValues;
}

VkResult VkGPURenderPass::CreateRenderPass() {
    if (gpuCtx == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    const VkResult result = VkGPUHelper::CreateRenderPass(gpuCtx->GetCurrentDevice(),
                                                          attachments,
                                                          dependencies,
                                                          subPasses,
                                                          &renderPass);
    if (result != VK_SUCCESS) {
        Logger() << "Failed to create render pass!" << std::endl;
        return result;
    }
    return VK_SUCCESS;
}

void VkGPURenderPass::GPUCmdBeginRenderPass(const VkCommandBuffer &commandBuffer) const {
    VkGPUHelper::GPUCmdBeginRenderPass(commandBuffer, renderPass, framebuffer, renderArea, clearValues);
}

void VkGPURenderPass::GPUCmdEndRenderPass(const VkCommandBuffer &commandBuffer) {
    VkGPUHelper::GPUCmdEndRenderPass(commandBuffer);
}

void VkGPURenderPass::Destroy() const {
    if (renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(this->gpuCtx->GetCurrentDevice(), renderPass, nullptr);
    }
}
