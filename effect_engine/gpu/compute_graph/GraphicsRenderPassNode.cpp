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
                                               const std::vector<VkAttachmentDescription> &attachments,
                                               const std::vector<VkSubpassDependency> &dependencies,
                                               const std::vector<VkSubpassDescription> &subPasses,
                                               const float width,
                                               const float height) {
    this->gpuCtx = gpuCtx;
    this->name = name;
    this->width = width;
    this->height = height;
    this->attachments = attachments;
    this->subPassDependencies = dependencies;
    this->subPasses = subPasses;
}

VkResult GraphicsRenderPassNode::CreateComputeGraphNode() {
    VkRect2D renderArea;
    renderArea.extent.width = this->width;
    renderArea.extent.height = this->height;
    renderArea.offset.x = 0;
    renderArea.offset.y = 0;
    // TODO: fix me
    std::vector<VkClearValue> clearValues;
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
