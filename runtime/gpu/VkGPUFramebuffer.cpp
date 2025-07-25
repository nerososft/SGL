//
// Created by neo on 25-4-13.
//

#include "VkGPUFramebuffer.h"

#include "VkGPUBuffer.h"
#include "VkGPUHelper.h"
#include "VkGPURenderPass.h"
#include "log/Log.h"

VkGPUFramebuffer::VkGPUFramebuffer(
    const std::shared_ptr<VkGPUContext> &gpuCtx, const float width,
    const float height, const std::shared_ptr<VkGPURenderPass> &renderPass) {
  this->gpuCtx = gpuCtx;
  this->width = width;
  this->height = height;
  this->renderPass = renderPass;
}

VkResult
VkGPUFramebuffer::CreateFramebuffer(std::vector<uint32_t> queueFamilies) {
  if (this->renderPass == nullptr || this->gpuCtx == nullptr) {
    Logger() << Logger::ERROR << "render pass or gpu context is NULL!"
             << std::endl;
    return VK_ERROR_INITIALIZATION_FAILED;
  }
  VkResult result = VK_SUCCESS;

  const VkPhysicalDeviceMemoryProperties memoryProperties =
      this->gpuCtx->GetMemoryProperties();
  result = VkGPUHelper::CreateImageAndBindMem(
      this->gpuCtx->GetCurrentDevice(), width, height, VK_IMAGE_TYPE_2D,
      VK_FORMAT_R8G8B8A8_SRGB,
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
      VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, &memoryProperties,
      queueFamilies, &colorImage, &colorImageMemory);
  if (result != VK_SUCCESS) {
    Logger() << Logger::ERROR << "failed to create color image!" << std::endl;
    return result;
  }

  result = VkGPUHelper::CreateImageView(
      this->gpuCtx->GetCurrentDevice(), this->colorImage, VK_IMAGE_VIEW_TYPE_2D,
      VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT,
      &this->colorImageView);
  if (result != VK_SUCCESS) {
    Logger() << Logger::ERROR << "failed to create color image view!"
             << std::endl;
    return result;
  }
  framebufferAttachments.push_back(this->colorImageView);

  result = VkGPUHelper::CreateImageAndBindMem(
      this->gpuCtx->GetCurrentDevice(), width, height, VK_IMAGE_TYPE_2D,
      VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, &memoryProperties,
      queueFamilies, &depthImage, &depthImageMemory);
  if (result != VK_SUCCESS) {
    Logger() << Logger::ERROR << "failed to create depth image!" << std::endl;
    return result;
  }

  result = VkGPUHelper::CreateImageView(
      this->gpuCtx->GetCurrentDevice(), this->depthImage, VK_IMAGE_VIEW_TYPE_2D,
      VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT, &this->depthImageView);
  if (result != VK_SUCCESS) {
    Logger() << Logger::ERROR << "failed to create depth image view!"
             << std::endl;
    return result;
  }
  framebufferAttachments.push_back(this->depthImageView);

  result = VkGPUHelper::CreateFramebuffer(
      this->gpuCtx->GetCurrentDevice(), static_cast<uint32_t>(width),
      static_cast<uint32_t>(height), framebufferAttachments,
      this->renderPass->GetRenderPass(), &this->framebuffer);
  if (result != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to create framebuffer!" << std::endl;
    return result;
  }
  return result;
}

void VkGPUFramebuffer::Destroy() {
  if (this->framebuffer != VK_NULL_HANDLE) {
    vkDestroyFramebuffer(this->gpuCtx->GetCurrentDevice(), this->framebuffer,
                         nullptr);
    this->framebuffer = VK_NULL_HANDLE;
  }
  if (this->depthImageMemory != VK_NULL_HANDLE) {
    vkFreeMemory(this->gpuCtx->GetCurrentDevice(), this->depthImageMemory,
                 nullptr);
    this->depthImageMemory = VK_NULL_HANDLE;
  }
  if (this->colorImageMemory != VK_NULL_HANDLE) {
    vkFreeMemory(this->gpuCtx->GetCurrentDevice(), this->colorImageMemory,
                 nullptr);
    this->colorImageMemory = VK_NULL_HANDLE;
  }
  if (this->depthImageView != VK_NULL_HANDLE) {
    vkDestroyImageView(this->gpuCtx->GetCurrentDevice(), this->depthImageView,
                       nullptr);
    this->depthImageView = VK_NULL_HANDLE;
  }
  if (this->colorImageView != VK_NULL_HANDLE) {
    vkDestroyImageView(this->gpuCtx->GetCurrentDevice(), this->colorImageView,
                       nullptr);
    this->colorImageView = VK_NULL_HANDLE;
  }
  if (this->colorImage != VK_NULL_HANDLE) {
    vkDestroyImage(this->gpuCtx->GetCurrentDevice(), this->colorImage, nullptr);
    this->colorImage = VK_NULL_HANDLE;
  }
  if (this->depthImage != VK_NULL_HANDLE) {
    vkDestroyImage(this->gpuCtx->GetCurrentDevice(), this->depthImage, nullptr);
    this->depthImage = VK_NULL_HANDLE;
  }
}
