//
// Created by neo on 25-4-13.
//

#ifndef VKGPUFRAMEBUFFER_H
#define VKGPUFRAMEBUFFER_H
#include <memory>

#include "VkGPUContext.h"
#include "VkGPURenderPass.h"

class VkGPUFramebuffer {
  std::shared_ptr<VkGPUContext> gpuCtx;
  std::shared_ptr<VkGPURenderPass> renderPass = nullptr;

  VkFramebuffer framebuffer = VK_NULL_HANDLE;

  VkImage colorImage = VK_NULL_HANDLE;
  VkDeviceMemory colorImageMemory = VK_NULL_HANDLE;
  VkImageView colorImageView = VK_NULL_HANDLE;

  VkImage depthImage = VK_NULL_HANDLE;
  VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
  VkImageView depthImageView = VK_NULL_HANDLE;

  std::vector<VkImageView> framebufferAttachments;

  float width = 0.0f;
  float height = 0.0f;

public:
  explicit VkGPUFramebuffer(const std::shared_ptr<VkGPUContext> &gpuCtx,
                            float width, float height,
                            const std::shared_ptr<VkGPURenderPass> &renderPass);

  ~VkGPUFramebuffer() = default;

  VkResult CreateFramebuffer(std::vector<uint32_t> queueFamilies);

  [[nodiscard]] VkImageView GetColorImageView() const {
    return this->colorImageView;
  }
  [[nodiscard]] VkImage GetColorImage() const { return this->colorImage; }
  [[nodiscard]] VkImageView GetDepthImageView() const {
    return this->depthImageView;
  }
  [[nodiscard]] VkImage GetDepthImage() const { return this->depthImage; }
  [[nodiscard]] VkFramebuffer GetFramebuffer() const {
    return this->framebuffer;
  }

  void Destroy();
};

#endif // VKGPUFRAMEBUFFER_H
