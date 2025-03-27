//
// Created by neo on 2025/3/25.
//

#ifndef VKGPURENDERPASS_H
#define VKGPURENDERPASS_H
#include <memory>
#include <vulkan/vulkan_core.h>

#include "VkGPUContext.h"


class VkGPURenderPass {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;

    std::vector<VkAttachmentDescription> attachments;
    std::vector<VkSubpassDependency> dependencies;
    std::vector<VkSubpassDescription> subPasses;

    VkRect2D renderArea;
    std::vector<VkClearValue> clearValues;

public:
    explicit VkGPURenderPass(const std::shared_ptr<VkGPUContext> &gpuCtx,
                             const std::vector<VkAttachmentDescription> &attachments,
                             const std::vector<VkSubpassDependency> &dependencies,
                             const std::vector<VkSubpassDescription> &subPasses,
                             VkRect2D renderArea,
                             const std::vector<VkClearValue> &clearValues);

    ~VkGPURenderPass() = default;

    VkResult CreateRenderPass();

    void GPUCmdBeginRenderPass(const VkCommandBuffer &commandBuffer) const;

    static void GPUCmdEndRenderPass(const VkCommandBuffer &commandBuffer);

    void Destroy() const;

    [[nodiscard]] VkRenderPass GetRenderPass() const { return renderPass; }
};

#endif //VKGPURENDERPASS_H
