//
// Created by neo on 25-4-11.
//

#ifndef RENDERER_H
#define RENDERER_H
#include "core/gpu/VkGPUBuffer.h"
#include "core/gpu/VkGPUContext.h"
#include "core/gpu/VkGPUFramebuffer.h"
#include "core/gpu/compute_graph/ComputeGraph.h"
#include "core/gpu/compute_graph/GraphicsPipelineNode.h"
#include "core/gpu/compute_graph/GraphicsRenderPassNode.h"
#include "core/gpu/compute_graph/ImageToBufferCopyNode.h"
#include <glm/glm.hpp>

#include "RendererCamera.h"
#include "RendererLight.h"
#include "RendererMesh.h"
#include "core/gpu/VkGPUSwapChain.h"

struct FrameInfo {
    uint32_t frameIndex;
    uint64_t timestamp;
};

enum RenderMode {
    RENDER_MODE_ONSCREEN,
    RENDER_MODE_OFFSCREEN
};

class Renderer {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> subComputeGraph = nullptr;

    std::shared_ptr<ImageToBufferCopyNode> offScreenCopyNode = nullptr;
    std::shared_ptr<VkGPUBuffer> offScreenBuffer = nullptr;

    std::shared_ptr<GraphicsRenderPassNode> mainRenderPassNode = nullptr;
    std::shared_ptr<GraphicsPipelineNode> graphicsPipelineNode = nullptr;
    std::shared_ptr<VkGPUFramebuffer> framebuffer = nullptr;

    RenderMode renderMode = RENDER_MODE_ONSCREEN;

    FrameInfo frameInfo{};

    uint32_t width = 768;
    uint32_t height = 768;

    std::vector<std::shared_ptr<RendererMesh> > rendererMeshes;
    std::vector<std::shared_ptr<RendererLight> > rendererLights;
    std::shared_ptr<RendererCamera> camera = nullptr;

    std::shared_ptr<VkGPUSwapChain> swapChain = nullptr;
    VkCommandBuffer presentCmdBuffer = VK_NULL_HANDLE;
    VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence renderFinishedFence = VK_NULL_HANDLE;

    std::function<bool(Renderer *rdr)> onLoadScene = nullptr;
    std::function<bool(Renderer *rdr)> onRendererReady = nullptr;

    bool InitCamera();

    bool InitLights();

public:
    Renderer(uint32_t width, uint32_t height);

    bool AddDrawElement(const std::vector<Vertex> &vertexData,
                        const std::vector<uint32_t> &indicesData,
                        const Material &material,
                        const glm::mat4 &transform);

    [[nodiscard]] bool ConstructMainGraphicsPipeline();

    bool Init();

    bool Init(const std::vector<const char *> &requiredExtensions, VkSurfaceKHR (*GetSurface)(VkInstance instance));

    [[nodiscard]] VkResult RenderFrame();

    [[nodiscard]] VkResult Present() const;

    ~Renderer() = default;

    void RenderFrameOffScreen(const std::string &path);

    std::shared_ptr<VkGPUContext> &GetGPUContext() { return gpuCtx; }

    void Update() const;

    void SetOnLoadScene(const std::function<bool(Renderer *dr)> &loadSceneFunc) {
        this->onLoadScene = loadSceneFunc;
    }

    void SetOnRendererReady(const std::function<bool(Renderer *dr)> &onRendererReady) {
        this->onRendererReady = onRendererReady;
    }

    std::shared_ptr<RendererCamera> GetCamera() { return camera; }
};

#endif //RENDERER_H
