//
// Created by neo on 2025/2/9.
//

#ifndef GRAPHICSAPP_H
#define GRAPHICSAPP_H
#include <memory>

#include "core/renderer/renderer/Renderer.h"
#include "core/renderer/renderer/RendererLight.h"
#include "event/IEventHandler.h"
#include "window/IWindow.h"

static std::shared_ptr<IWindow> window = nullptr;

class GraphicsApp final : public IEventHandler {
  std::shared_ptr<Renderer> renderer = nullptr;

  std::shared_ptr<GraphicsPipelineNode> graphicsPipelineNode = nullptr;
  std::vector<std::shared_ptr<RendererMesh>> rendererMeshes;
  std::vector<std::shared_ptr<RendererLight>> rendererLights;
  std::shared_ptr<RendererCamera> camera = nullptr;
  FrameInfo frameInfo{};
  uint64_t lastRenderTimeMs = 0;
  uint64_t lastRenderFrame = 0;
  uint64_t fps = 0;

  bool running = true;

  const char *windowTitle = "Engine";
  uint32_t frameCount = 0;
  uint32_t windowPosX = 10;
  uint32_t windowPosY = 10;
  uint32_t windowWidth = 1024;
  uint32_t windowHeight = 768;

public:
  GraphicsApp() = default;

  ~GraphicsApp() override = default;

  bool ConstructRendererPipeline();

  bool InitCamera();

  bool InitLights();

  void Init();

  void Update() const;

  void Run();

  [[nodiscard]] uint64_t GetFPS() const;

  void CaptureFrame(const std::string &path) const;

  void SetWindowSize(uint32_t width, uint32_t height);

  void SetWindowPos(uint32_t x, uint32_t y);

  void SetWindowTitle(const char *title);

  void OnKeyUp(uint32_t key) override;

  void OnKeyDown(uint32_t key) override;

  void OnMouseDown(uint32_t key) override;

  void OnMouseUp(uint32_t key) override;

  void OnMouseMove(uint32_t x, uint32_t y) override;
};

#endif // GRAPHICSAPP_H
