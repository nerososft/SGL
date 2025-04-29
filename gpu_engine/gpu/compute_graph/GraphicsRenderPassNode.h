//
// Created by neo on 2025/3/27.
//

#ifndef GRAPHICSRENDERPASSNODE_H
#define GRAPHICSRENDERPASSNODE_H
#include "IComputeGraphNode.h"
#include "gpu_engine/gpu/VkGPUContext.h"
#include "gpu_engine/gpu/VkGPUFramebuffer.h"
#include "gpu_engine/gpu/VkGPURenderPass.h"

class GraphicsRenderPassNode final : public IComputeGraphNode {
    float width = 1.0f;
    float height = 1.0f;
    std::vector<VkAttachmentDescription> attachments;
    std::vector<VkSubpassDependency> subPassDependencies;
    std::vector<VkSubpassDescription> subPasses;
    std::vector<VkClearValue> clearValues;

    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<VkGPURenderPass> renderPass = nullptr;
    std::shared_ptr<VkGPUFramebuffer> framebuffer = nullptr;

public:
    GraphicsRenderPassNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           const std::string &name,
                           const std::vector<VkAttachmentDescription> &attachments,
                           const std::vector<VkSubpassDependency> &dependencies,
                           const std::vector<VkSubpassDescription> &subPasses,
                           float width,
                           float height,
                           const std::vector<VkClearValue> &clearValues);

    ~GraphicsRenderPassNode() override = default;

    VkResult CreateComputeGraphNode() override;

    void Compute(VkCommandBuffer commandBuffer) override;

    void Destroy() override;

    void SetFramebuffer(const std::shared_ptr<VkGPUFramebuffer> &framebuffer) { this->framebuffer = framebuffer; }

    std::shared_ptr<VkGPURenderPass> GetRenderPass() { return this->renderPass; }
};


#endif //GRAPHICSRENDERPASSNODE_H
