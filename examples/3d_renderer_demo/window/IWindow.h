//
// Created by neo on 2025/2/9.
//

#ifndef WINDOW_H
#define WINDOW_H

#include <vector>
#include <vulkan/vulkan_core.h>

#include "../event/IEventHandler.h"

struct Surface {
  int width;
  int height;

  union {
    VkSurfaceKHR vkSurface;
  } surface;
};

struct SurfaceGetParams {
  union {
    VkInstance vkInstance;
  } params;
};

class IWindow {
  IEventHandler *handler = nullptr;

public:
  explicit IWindow(IEventHandler *handler);

  virtual ~IWindow() = default;

  virtual void CreateWindow(uint32_t posX, uint32_t posY, uint32_t width,
                            uint32_t height, const char *title) = 0;

  virtual bool ShouldClose() = 0;

  virtual void PollEvent() = 0;

  virtual void Render() = 0;

  virtual std::vector<const char *> GetRendererRequiredExtensions() = 0;

  virtual Surface GetSurface(SurfaceGetParams params) = 0;
};

#endif // WINDOW_H
