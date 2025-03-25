//
// Created by neo on 2025/3/25.
//

#include "VkGPURenderPass.h"

VkGPURenderPass::VkGPURenderPass(const std::shared_ptr<VkGPUContext> &gpuCtx) {
    this->gpuCtx = gpuCtx;
}

VkResult VkGPURenderPass::CreateRenderPass() {
    // TODO:
    return VK_SUCCESS;
}

void VkGPURenderPass::Destroy() const {
    if (renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(this->gpuCtx->GetCurrentDevice(), renderPass, nullptr);
    }
}
