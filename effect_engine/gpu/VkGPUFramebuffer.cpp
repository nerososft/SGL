//
// Created by neo on 25-4-13.
//

#include "VkGPUFramebuffer.h"

#include "VkGPUBuffer.h"
#include "VkGPUHelper.h"
#include "VkGPURenderPass.h"
#include "effect_engine/log/Log.h"

VkGPUFramebuffer::VkGPUFramebuffer(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                   const float width,
                                   const float height,
                                   const std::shared_ptr<VkGPURenderPass> &renderPass) {
    this->gpuCtx = gpuCtx;
    this->width = width;
    this->height = height;
    this->renderPass = renderPass;
}

VkResult VkGPUFramebuffer::CreateFramebuffer(std::vector<uint32_t> queueFamilies) {
    if (this->renderPass == nullptr || this->gpuCtx == nullptr) {
        Logger() << Logger::ERROR << "render pass or gpu context is NULL!" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    VkResult result = VK_SUCCESS;

    result = VkGPUHelper::CreateImage(this->gpuCtx->GetCurrentDevice(),
                                      this->width,
                                      this->height,
                                      VK_IMAGE_TYPE_2D,
                                      VK_FORMAT_R8G8B8A8_SRGB,
                                      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                      VK_SHARING_MODE_EXCLUSIVE,
                                      queueFamilies,
                                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                      &this->colorImage);
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "failed to create color image!" << std::endl;
        return result;
    }

    // TODO:

    framebufferAttachments.push_back(this->colorImageView);
    framebufferAttachments.push_back(this->depthImageView);
    result = VkGPUHelper::CreateFramebuffer(this->gpuCtx->GetCurrentDevice(),
                                            width,
                                            height,
                                            framebufferAttachments,
                                            this->renderPass->GetRenderPass(),
                                            &this->framebuffer);
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create framebuffer!" << std::endl;
        return result;
    }
    return result;
}

void VkGPUFramebuffer::Destroy() {
    // TODO:
}
