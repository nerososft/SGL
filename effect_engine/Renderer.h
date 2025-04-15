//
// Created by neo on 25-4-11.
//

#ifndef RENDERER_H
#define RENDERER_H
#include "gpu/VkGPUBuffer.h"
#include "gpu/VkGPUContext.h"
#include "gpu/VkGPUFramebuffer.h"
#include "gpu/compute_graph/ComputeGraph.h"
#include "gpu/compute_graph/GraphicsPipelineNode.h"
#include "gpu/compute_graph/GraphicsRenderPassNode.h"
#include "gpu/compute_graph/ImageToBufferCopyNode.h"
#include <glm/glm.hpp>

#include "gpu/VkGPUSwapChain.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
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

    std::shared_ptr<VkGPUBuffer> indicesBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> vertexBuffer = nullptr;

public:
    Renderer() = default;

    [[nodiscard]] bool ConstructMainGraphicsPipeline();

    bool Init();

    [[nodiscard]] VkResult RenderFrame() const;

    ~Renderer() = default;

    void RenderFrameOffScreen(const std::string &path) const;
};

#endif //RENDERER_H
