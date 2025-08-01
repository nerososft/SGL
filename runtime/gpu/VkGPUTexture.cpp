//
// Created by neo on 25-8-1.
//

#include "VkGPUTexture.h"

#include "VkGPUHelper.h"
#include "log/Log.h"
VkGPUTexture::VkGPUTexture(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           const float width, const float height) {
  this->width = width;
  this->height = height;
  this->gpuCtx = gpuCtx;
}

VkResult VkGPUTexture::CreateTexture() {
  const std::vector<uint32_t> queueFamilies = {0};
  VkResult ret = VK_SUCCESS;
  ret = VkGPUHelper::CreateImage(
      this->gpuCtx->GetCurrentDevice(), this->width, this->height,
      VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB,
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VK_SHARING_MODE_EXCLUSIVE, queueFamilies, VK_IMAGE_LAYOUT_UNDEFINED,
      &this->textureImage);
  if (ret != VK_SUCCESS) {
    Logger() << "failed to create texture image" << std::endl;
    return ret;
  }

  imageBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
  ret = imageBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_LOCAL,
                                     this->width * this->height *
                                         sizeof(uint32_t));
  if (ret != VK_SUCCESS) {
    Logger() << "failed to allocate image buffer" << std::endl;
  }

  ret = vkBindImageMemory(this->gpuCtx->GetCurrentDevice(), this->textureImage,
                          imageBuffer->GetDeviceMemory(), 0);
  if (ret != VK_SUCCESS) {
    Logger() << "failed to bind image memory" << std::endl;
    return ret;
  }

  ret = VkGPUHelper::CreateImageView(
      this->gpuCtx->GetCurrentDevice(), this->textureImage,
      VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT,
      &this->textureImageView);
  if (ret != VK_SUCCESS) {
    Logger() << "failed to create texture imageview" << std::endl;
    return ret;
  }

  ret = VkGPUHelper::CreateSampler(this->gpuCtx->GetCurrentDevice(),
                                   &this->textureSampler);
  if (ret != VK_SUCCESS) {
    Logger() << "failed to create texture sampler" << std::endl;
    return ret;
  }

  return ret;
}

void VkGPUTexture::Destroy() {
  if (textureImage != VK_NULL_HANDLE) {
    vkDestroyImage(this->gpuCtx->GetCurrentDevice(), textureImage, nullptr);
    textureImage = VK_NULL_HANDLE;
  }
  if (textureImageView != VK_NULL_HANDLE) {
    vkDestroyImageView(this->gpuCtx->GetCurrentDevice(), textureImageView,
                       nullptr);
    textureImageView = VK_NULL_HANDLE;
  }
  if (textureSampler != VK_NULL_HANDLE) {
    vkDestroySampler(this->gpuCtx->GetCurrentDevice(), textureSampler, nullptr);
    textureSampler = VK_NULL_HANDLE;
  }
  if (this->imageBuffer != VK_NULL_HANDLE) {
    this->imageBuffer->Destroy();
  }
}