//
// Created by neo on 25-4-11.
//

#ifndef RENDERER_H
#define RENDERER_H
#include "../../gpu_engine/gpu/VkGPUBuffer.h"
#include "../../gpu_engine/gpu/VkGPUContext.h"
#include "../../gpu_engine/gpu/VkGPUFramebuffer.h"
#include "../../gpu_engine/gpu/compute_graph/ComputeGraph.h"
#include "../../gpu_engine/gpu/compute_graph/GraphicsPipelineNode.h"
#include "../../gpu_engine/gpu/compute_graph/GraphicsRenderPassNode.h"
#include "../../gpu_engine/gpu/compute_graph/ImageToBufferCopyNode.h"
#include <glm/glm.hpp>

#include "RendererLight.h"
#include "RendererMesh.h"
#include "../../gpu_engine/gpu/VkGPUSwapChain.h"

struct FrameInfo {
    uint32_t frameIndex;
    uint64_t timestamp;
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

    std::shared_ptr<VkGPUSwapChain> swapChain = nullptr;

    FrameInfo frameInfo{};

    uint32_t width = 768;
    uint32_t height = 768;

    glm::mat4 worldMatrix = glm::mat4(1.0f);
    std::vector<std::shared_ptr<RendererMesh> > rendererMeshes;
    std::vector<std::shared_ptr<RendererLight> > rendererLights;

    VkCommandBuffer presentCmdBuffer = VK_NULL_HANDLE;
    VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence renderFinishedFence = VK_NULL_HANDLE;

    bool InitCamera();

    bool InitLights();

public:
    Renderer(uint32_t width, uint32_t height);

    bool AddDrawElement(const std::vector<Vertex> &vertexData,
                        const std::vector<uint32_t> &indicesData,
                        const Material &material,
                        const glm::mat4 &transform);

    [[nodiscard]] bool ConstructMainGraphicsPipeline();

    bool Init(const std::vector<const char *> &requiredExtensions, VkSurfaceKHR (*GetSurface)(VkInstance instance));

    [[nodiscard]] VkResult RenderFrame();

    [[nodiscard]] VkResult Present() const;

    ~Renderer() = default;

    void RenderFrameOffScreen(const std::string &path);

    std::shared_ptr<VkGPUContext> &GetGPUContext() { return gpuCtx; }

    void Update();
};

#endif //RENDERER_H
