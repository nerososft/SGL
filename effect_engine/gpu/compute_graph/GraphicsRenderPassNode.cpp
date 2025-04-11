//
// Created by neo on 2025/3/27.
//

#include "GraphicsRenderPassNode.h"

#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/gpu/VkGPUHelper.h"
#include "effect_engine/log/Log.h"

GraphicsRenderPassNode::GraphicsRenderPassNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                               const std::string &name,
                                               float width,
                                               float height) {
    this->gpuCtx = gpuCtx;
    this->name = name;
    this->width = width;
    this->height = height;
}

VkResult GraphicsRenderPassNode::CreateComputeGraphNode() {
    // TODO:
    const std::vector<VkAttachmentDescription> attachments;
    const std::vector<VkSubpassDependency> dependencies;
    const std::vector<VkSubpassDescription> subPasses;
    VkRect2D renderArea;
    const std::vector<VkClearValue> clearValues;
    renderPass = std::make_shared<VkGPURenderPass>(gpuCtx,
                                                   attachments,
                                                   dependencies,
                                                   subPasses,
                                                   renderArea,
                                                   clearValues);
    const VkResult ret = renderPass->CreateRenderPass();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create render pass, err = " << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}

void GraphicsRenderPassNode::Compute(const VkCommandBuffer commandBuffer) {
    this->renderPass->GPUCmdBeginRenderPass(commandBuffer, framebuffer);
    if (!this->dependencies.empty()) {
        for (const auto &dependence: this->dependencies) {
            Logger() << "Node: " << name << " Depend On:" << dependence->GetName() << std::endl;
            dependence->Compute(commandBuffer);
        }
    }
    this->renderPass->GPUCmdEndRenderPass(commandBuffer);
}

void GraphicsRenderPassNode::Destroy() {
    this->renderPass->Destroy();
}
