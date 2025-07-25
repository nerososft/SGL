//
// Created by neo on 2025/2/9.
//

#include "GLFWWindowImpl.h"

#include <iostream>

#include "runtime/log/Log.h"

static IEventHandler *gHandler = nullptr;

GLFWWindowImpl::GLFWWindowImpl(IEventHandler *handler) : IWindow(handler) {
  if (!glfwInit()) {
    glfwInitialized = false;
    Logger() << "Failed to initialize GLFW" << std::endl;
  }
  glfwInitialized = true;
  gHandler = handler;
}

void GLFWWindowImpl::CreateWindow(uint32_t posX, uint32_t posY,
                                  const uint32_t width, const uint32_t height,
                                  const char *title) {
  if (!glfwInitialized) {
    Logger() << "GLFW not initialized!" << std::endl;
    return;
  }
  if (!glfwVulkanSupported()) {
    Logger() << "GLFW Vulkan not supported!" << std::endl;
    return;
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  Logger() << "Creating window '" << title << "', width=" << width
           << ", height=" << height << std::endl;
  this->window =
      glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title,
                       nullptr, nullptr);
  if (!this->window) {
    Logger() << "Failed to create GLFW window!" << std::endl;
    return;
  }

  glfwSetKeyCallback(this->window, [](GLFWwindow *window, int key, int scancode,
                                      int action, int mods) {
    if (gHandler == nullptr) {
      Logger() << "GLFW window has no handler!" << std::endl;
      return;
    }
    if (action == GLFW_PRESS) {
      gHandler->OnKeyDown(key);
    }
    if (action == GLFW_RELEASE) {
      gHandler->OnKeyUp(key);
    }
  });

  glfwSetCursorPosCallback(
      this->window, [](GLFWwindow *window, double xpos, double ypos) {
        if (gHandler == nullptr) {
          Logger() << "GLFW window has no handler!" << std::endl;
          return;
        }
        gHandler->OnMouseMove(xpos, ypos);
      });

  glfwSetMouseButtonCallback(
      this->window, [](GLFWwindow *window, int button, int action, int mods) {
        if (gHandler == nullptr) {
          Logger() << "GLFW window has no handler!" << std::endl;
          return;
        }
        if (action == GLFW_PRESS) {
          gHandler->OnMouseDown(button);
        }
        if (action == GLFW_RELEASE) {
          gHandler->OnMouseUp(button);
        }
      });
}

GLFWWindowImpl::~GLFWWindowImpl() { glfwTerminate(); }

bool GLFWWindowImpl::ShouldClose() {
  return glfwWindowShouldClose(this->window);
}

void GLFWWindowImpl::PollEvent() { glfwPollEvents(); }

void GLFWWindowImpl::Render() { glfwSwapBuffers(this->window); }

std::vector<const char *> GLFWWindowImpl::GetRendererRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **extensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  Logger() << "GLFW window extensions(count:" << glfwExtensionCount
           << "): " << std::endl;
  std::vector<const char *> requiredExtensions;
  requiredExtensions.reserve(glfwExtensionCount);
  for (int i = 0; i < glfwExtensionCount; i++) {
    Logger() << "GLFW window extension(" << i << "): " << extensions[i]
             << std::endl;
    requiredExtensions.push_back(extensions[i]);
  }
  return requiredExtensions;
}

Surface GLFWWindowImpl::GetSurface(const SurfaceGetParams params) {
  Surface surface{};

  VkSurfaceKHR vkSurface = VK_NULL_HANDLE;
  glfwCreateWindowSurface(params.params.vkInstance, this->window, nullptr,
                          &vkSurface);
  surface.surface.vkSurface = vkSurface;

  glfwGetFramebufferSize(window, &surface.width, &surface.height);
  return surface;
}
