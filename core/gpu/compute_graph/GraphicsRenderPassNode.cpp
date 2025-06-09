//
// Created by neo on 2025/3/27.
//

#include "GraphicsRenderPassNode.h"

#ifdef OS_OPEN_HARMONY
#include <core/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "gpu/VkGPUHelper.h"
#include "log/Log.h"

GraphicsRenderPassNode::GraphicsRenderPassNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                               const std::string &name,
                                               const std::vector<VkAttachmentDescription> &attachments,
                                               const std::vector<VkSubpassDependency> &dependencies,
                                               const std::vector<VkSubpassDescription> &subPasses,
                                               const float width,
                                               const float height,
                                               const std::vector<VkClearValue> &clearValues) {
    this->gpuCtx = gpuCtx;
    this->name = name;
    this->width = width;
    this->height = height;
    this->attachments = attachments;
    this->subPassDependencies = dependencies;
    this->subPasses = subPasses;
    this->clearValues = clearValues;
}

VkResult GraphicsRenderPassNode::CreateComputeGraphNode() {
    VkRect2D renderArea;
    renderArea.extent.width = static_cast<uint32_t>(this->width);
    renderArea.extent.height = static_cast<uint32_t>(this->height);
    renderArea.offset.x = 0;
    renderArea.offset.y = 0;
    this->renderPass = std::make_shared<VkGPURenderPass>(gpuCtx,
                                                         attachments,
                                                         subPassDependencies,
                                                         subPasses,
                                                         renderArea,
                                                         clearValues);
    if (this->renderPass == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        return VK_ERROR_UNKNOWN;
    }
    const VkResult ret = renderPass->CreateRenderPass();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create render pass, err = " << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}

void GraphicsRenderPassNode::Compute(const VkCommandBuffer commandBuffer) {
    if (framebuffer == nullptr) {
        Logger() << Logger::ERROR << "framebuffer not created!" << std::endl;
        return;
    }
    const std::vector<VkViewport> viewports = {
        {
            .x = 0,
            .y = 0,
            .width = this->width,
            .height = this->height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        }
    };

    const std::vector<VkRect2D> scissors = {
        {
            .offset = {0, 0},
            .extent = {static_cast<uint32_t>(this->width), static_cast<uint32_t>(this->height)},
        }
    };
    vkCmdSetViewport(commandBuffer, 0, viewports.size(), viewports.data());
    vkCmdSetScissor(commandBuffer, 0, scissors.size(), scissors.data());
    this->renderPass->GPUCmdBeginRenderPass(commandBuffer, framebuffer->GetFramebuffer());
    if (!this->dependencies.empty()) {
        for (const auto &dependence: this->dependencies) {
            Logger() << "Node: " << name << " Depend On:" << dependence->GetName() << std::endl;
            dependence->Compute(commandBuffer);
        }
    }
    VkGPURenderPass::GPUCmdEndRenderPass(commandBuffer);
}

void GraphicsRenderPassNode::Destroy() {
    this->renderPass->Destroy();
}
