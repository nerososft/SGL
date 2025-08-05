//
// Created by neo on 25-8-1.
//

#ifndef VKGPUTEXTURE_H
#define VKGPUTEXTURE_H
#include "VkGPUBuffer.h"
#include "VkGPUContext.h"

#include <memory>

class VkGPUTexture {
  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
  VkImage textureImage = VK_NULL_HANDLE;
  VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
  VkImageView textureImageView = VK_NULL_HANDLE;
  VkSampler textureSampler = VK_NULL_HANDLE;
  std::shared_ptr<VkGPUBuffer> imageBindBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> imageStageBuffer = nullptr;

  float width;
  float height;

public:
  VkGPUTexture(const std::shared_ptr<VkGPUContext> &gpuCtx, float width,
               float height);

  [[nodiscard]] VkImage GetTextureImage() const { return textureImage; }

  [[nodiscard]] VkImageView GetTextureImageView() const {
    return textureImageView;
  }

  [[nodiscard]] VkSampler GetTextureSampler() const { return textureSampler; }

  std::shared_ptr<VkGPUBuffer> GetImageStageBuffer() {
    return imageStageBuffer;
  }

  ~VkGPUTexture() = default;

  VkResult CreateTexture();

  void Destroy();
};

#endif // VKGPUTEXTURE_H
