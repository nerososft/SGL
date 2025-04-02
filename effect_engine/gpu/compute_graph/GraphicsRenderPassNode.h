//
// Created by neo on 2025/3/27.
//

#ifndef GRAPHICSRENDERPASSNODE_H
#define GRAPHICSRENDERPASSNODE_H
#include "IComputeGraphNode.h"
#include "effect_engine/gpu/VkGPUContext.h"
#include "effect_engine/gpu/VkGPURenderPass.h"

class GraphicsRenderPassNode final : IComputeGraphNode {
    float width = 1.0f;
    float height = 1.0f;

    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<VkGPURenderPass> renderPass = nullptr;
    VkFramebuffer framebuffer = nullptr;

public:
    GraphicsRenderPassNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           const std::string &name,
                           float width,
                           float height);

    ~GraphicsRenderPassNode() override = default;

    VkResult CreateComputeGraphNode() override;

    void Compute(VkCommandBuffer commandBuffer) override;

    void Destroy() override;
};


#endif //GRAPHICSRENDERPASSNODE_H
