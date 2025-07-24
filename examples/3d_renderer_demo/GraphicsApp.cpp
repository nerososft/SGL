//
// Created by neo on 2025/2/9.
//

#include "GraphicsApp.h"

#include "core/log/Log.h"
#include "scene/ModelLoader.h"
#include "window/impl/GLFWWindowImpl.h"

VkSurfaceKHR GetWindowSurface(const VkInstance instance) {
  const SurfaceGetParams params{.params = {
                                    .vkInstance = instance,
                                }};
  return window->GetSurface(params).surface.vkSurface;
}

void GraphicsApp::Init() {
  window = std::make_shared<GLFWWindowImpl>(this);
  const std::vector<const char *> requiredExtensions =
      window->GetRendererRequiredExtensions();
  renderer = std::make_shared<Renderer>(this->windowWidth, this->windowHeight);

  renderer->SetOnLoadScene([](Renderer *rdr) -> bool {
    const std::vector<std::shared_ptr<Mesh>> models =
        ModelLoader::LoadModel("../../../examples/3d_renderer_demo/assets/"
                               "builtin.models/Helmet/DamagedHelmet.gltf");
    for (auto &mesh : models) {
      if (!rdr->AddDrawElement(mesh->vertexData, mesh->indicesData,
                               mesh->material, mesh->transform)) {
        Logger() << "draw mesh add failed" << std::endl;
        return false;
      }
    }
    return true;
  });
  window->CreateWindow(this->windowPosX, this->windowPosY, this->windowWidth,
                       this->windowHeight, this->windowTitle);
  if (!renderer->Init(requiredExtensions, GetWindowSurface)) {
    Logger() << "Failed to initialize renderer" << std::endl;
  }
}

void GraphicsApp::Run() const {
  while (running && !window->ShouldClose()) {
    window->PollEvent();
    window->Render();
    renderer->Update();
    if (renderer->RenderFrame() != VK_SUCCESS) {
      return;
    }
    if (renderer->Present() != VK_SUCCESS) {
      return;
    }
  }
}

void GraphicsApp::CaptureFrame(const std::string &path) const {
  if (running && !window->ShouldClose()) {
    window->PollEvent();
    window->Render();
    renderer->RenderFrameOffScreen(path);
  }
}

void GraphicsApp::SetWindowSize(const uint32_t width, const uint32_t height) {
  this->windowWidth = width;
  this->windowHeight = height;
}

void GraphicsApp::SetWindowPos(const uint32_t x, const uint32_t y) {
  this->windowPosX = x;
  this->windowPosY = y;
}

void GraphicsApp::SetWindowTitle(const char *title) {
  this->windowTitle = title;
}

void GraphicsApp::OnKeyUp(const uint32_t key) {
  Logger() << "Key up: " << key << std::endl;
}

void GraphicsApp::OnKeyDown(const uint32_t key) {
  Logger() << "Key down: " << key << std::endl;
}

void GraphicsApp::OnMouseDown(const uint32_t key) {
  Logger() << "Mouse down: " << key << std::endl;
}

void GraphicsApp::OnMouseUp(const uint32_t key) {
  Logger() << "Mouse up: " << key << std::endl;
}

void GraphicsApp::OnMouseMove(const uint32_t x, const uint32_t y) {
  Logger() << "Mouse move: " << x << ", " << y << std::endl;
}
