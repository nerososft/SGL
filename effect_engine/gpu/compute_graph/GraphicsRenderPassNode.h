//
// Created by neo on 2025/3/27.
//

#ifndef GRAPHICSRENDERPASSNODE_H
#define GRAPHICSRENDERPASSNODE_H
#include "IComputeGraphNode.h"
#include "effect_engine/gpu/VkGPUContext.h"
#include "effect_engine/gpu/VkGPURenderPass.h"

class GraphicsRenderPassNode final : public IComputeGraphNode {
    float width = 1.0f;
    float height = 1.0f;
    std::vector<VkAttachmentDescription> attachments;
    std::vector<VkSubpassDependency> subPassDependencies;
    std::vector<VkSubpassDescription> subPasses;

    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<VkGPURenderPass> renderPass = nullptr;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;

public:
    GraphicsRenderPassNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           const std::string &name,
                           const std::vector<VkAttachmentDescription> &attachments,
                           const std::vector<VkSubpassDependency> &dependencies,
                           const std::vector<VkSubpassDescription> &subPasses,
                           float width,
                           float height);

    ~GraphicsRenderPassNode() override = default;

    VkResult CreateComputeGraphNode() override;

    void Compute(VkCommandBuffer commandBuffer) override;

    void Destroy() override;

    std::shared_ptr<VkGPURenderPass> GetRenderPass() { return this->renderPass; }
};


#endif //GRAPHICSRENDERPASSNODE_H
