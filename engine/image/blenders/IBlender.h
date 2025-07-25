//
// Created by neo on 2025/3/14.
//

#ifndef IBLEND_H
#define IBLEND_H
#include <vulkan/vulkan_core.h>

#include "core/gpu/VkGPUContext.h"
#include <memory>
typedef struct {
  uint32_t width;
  uint32_t height;
  uint32_t channels;
  uint32_t posX;
  uint32_t posY;
  VkDeviceSize bufferSize;
  VkBuffer storageBuffer;
} BlendImageInfo;

class IBlender {
public:
  IBlender() = default;

  virtual ~IBlender() = default;

  virtual VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                         BlendImageInfo baseImageInfo,
                         BlendImageInfo blendImageInfo,
                         VkBuffer outputBuffer) = 0;

  virtual void Destroy() = 0;
};

#endif // IBLEND_H
