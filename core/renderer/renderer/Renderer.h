//
// Created by neo on 25-4-11.
//

#ifndef RENDERER_H
#define RENDERER_H
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/VkGPUContext.h"
#include "runtime/gpu/VkGPUFramebuffer.h"
#include "runtime/gpu/compute_graph/ComputeGraph.h"
#include "runtime/gpu/compute_graph/GraphicsPipelineNode.h"
#include "runtime/gpu/compute_graph/GraphicsRenderPassNode.h"
#include "runtime/gpu/compute_graph/ImageToBufferCopyNode.h"
#include <glm/glm.hpp>

#include "RendererCamera.h"
#include "RendererMesh.h"
#include "runtime/gpu/VkGPUSwapChain.h"
#include "runtime/log/Log.h"

struct FrameInfo {
  uint32_t frameIndex;
  uint64_t timestamp;
};

enum RenderMode { RENDER_MODE_ONSCREEN, RENDER_MODE_OFFSCREEN };

class Renderer {
  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> subComputeGraph = nullptr;

  std::shared_ptr<ImageToBufferCopyNode> offScreenCopyNode = nullptr;
  std::shared_ptr<VkGPUBuffer> offScreenBuffer = nullptr;

  std::shared_ptr<GraphicsRenderPassNode> mainRenderPassNode = nullptr;
  std::shared_ptr<VkGPUFramebuffer> framebuffer = nullptr;

  RenderMode renderMode = RENDER_MODE_ONSCREEN;

  uint32_t width = 768;
  uint32_t height = 768;

  std::shared_ptr<VkGPUSwapChain> swapChain = nullptr;
  VkCommandBuffer presentCmdBuffer = VK_NULL_HANDLE;
  VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
  VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
  VkFence renderFinishedFence = VK_NULL_HANDLE;

public:
  Renderer(uint32_t width, uint32_t height);

  void AddRenderGraph(const std::shared_ptr<IComputeGraphNode> &graph) const;

  bool Init();

  bool Init(const std::vector<const char *> &requiredExtensions,
            VkSurfaceKHR (*GetSurface)(VkInstance instance));

  [[nodiscard]] VkResult RenderFrame();

  [[nodiscard]] VkResult Present() const;

  [[nodiscard]] std::shared_ptr<VkGPURenderPass> GetMainRenderPass() const {
    if (this->mainRenderPassNode == nullptr) {
      Logger() << Logger::ERROR << "Main render pass not initialized!"
               << std::endl;
      return nullptr;
    }
    return this->mainRenderPassNode->GetRenderPass();
  }

  std::shared_ptr<VkGPUContext> &GetGPUCtx() { return this->gpuCtx; }

  void RenderFrameOffScreen(const std::string &path);

  ~Renderer() = default;
};

#endif // RENDERER_H
