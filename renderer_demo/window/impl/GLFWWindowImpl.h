//
// Created by neo on 2025/2/9.
//

#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <vector>
#include <GLFW/glfw3.h>

#include "renderer_demo/event/IEventHandler.h"
#include "renderer_demo/window/IWindow.h"

class GLFWWindowImpl final : public IWindow {
    GLFWwindow *window = nullptr;
    bool glfwInitialized = false;

public:
    explicit GLFWWindowImpl(IEventHandler *handler);

    void CreateWindow(uint32_t posX, uint32_t posY, uint32_t width, uint32_t height, const char *title) override;

    ~GLFWWindowImpl() override;

    bool ShouldClose() override;

    void PollEvent() override;

    void Render() override;

    std::vector<const char *> GetRendererRequiredExtensions() override;

    Surface GetSurface(SurfaceGetParams params) override;
};


#endif //GLFWWINDOW_H
