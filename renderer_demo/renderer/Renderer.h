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

#include "../../gpu_engine/gpu/VkGPUSwapChain.h"
#include "renderer_demo/scene/Camera.h"

struct Material {
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
};

struct MatrixMVP {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

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

    uint32_t width = 1024;
    uint32_t height = 768;

    std::shared_ptr<Camera> camera = nullptr;

    MatrixMVP matrixMVP{};

    std::vector<std::shared_ptr<VkGPUBuffer> > indicesBuffers;
    std::vector<std::shared_ptr<VkGPUBuffer> > vertexBuffers;
    std::vector<std::shared_ptr<VkGPUBuffer> > uniformBuffers;

    VkCommandBuffer presentCmdBuffer = VK_NULL_HANDLE;
    VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence renderFinishedFence = VK_NULL_HANDLE;

public:
    Renderer() = default;

    bool AddDrawElement(const std::vector<Vertex> &vertexData,
                        const std::vector<uint32_t> &indicesData,
                        const Material &material);

    [[nodiscard]] bool ConstructMainGraphicsPipeline();

    bool Init(const std::vector<const char *> &requiredExtensions, VkSurfaceKHR (*GetSurface)(VkInstance instance));

    [[nodiscard]] VkResult RenderFrame();

    [[nodiscard]] VkResult Present() const;

    ~Renderer() = default;

    void RenderFrameOffScreen(const std::string &path);

    std::shared_ptr<VkGPUContext> &GetGPUContext() { return gpuCtx; }
};

#endif //RENDERER_H
