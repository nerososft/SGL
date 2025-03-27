//
// Created by neo on 2025/3/25.
//

#include "VkGPURenderPass.h"

#include "VkGPUHelper.h"
#include "effect_engine/log/Log.h"

VkGPURenderPass::VkGPURenderPass(const std::shared_ptr<VkGPUContext> &gpuCtx) {
    this->gpuCtx = gpuCtx;
}

VkResult VkGPURenderPass::CreateRenderPass() {
    if (gpuCtx == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // TODO:
    const std::vector<VkAttachmentDescription> attachments;
    const std::vector<VkSubpassDependency> dependencies;
    const std::vector<VkSubpassDescription> subPasses;
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


void VkGPURenderPass::Destroy() const {
    if (renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(this->gpuCtx->GetCurrentDevice(), renderPass, nullptr);
    }
}
