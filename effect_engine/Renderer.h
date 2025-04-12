//
// Created by neo on 25-4-11.
//

#ifndef RENDERER_H
#define RENDERER_H
#include "gpu/VkGPUContext.h"
#include "gpu/compute_graph/ComputeGraph.h"
#include "gpu/compute_graph/GraphicsPipelineNode.h"
#include "gpu/compute_graph/GraphicsRenderPassNode.h"

struct Vertex {
    float pos[3];
};

class Renderer {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> subComputeGraph = nullptr;
    std::shared_ptr<GraphicsRenderPassNode> mainRenderPassNode = nullptr;
    std::shared_ptr<GraphicsPipelineNode> graphicsPipelineNode = nullptr;

    uint32_t width = 1024;
    uint32_t height = 768;

public:
    Renderer() = default;

    [[nodiscard]] bool ConstructMainGraphicsPipeline();

    bool Init();

    void RenderFrame() const;

    ~Renderer() = default;
};

#endif //RENDERER_H
