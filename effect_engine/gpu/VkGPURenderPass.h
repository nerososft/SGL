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

public:
    explicit VkGPURenderPass(const std::shared_ptr<VkGPUContext> &gpuCtx);

    ~VkGPURenderPass() = default;

    VkResult CreateRenderPass();

    void Destroy() const;

    [[nodiscard]] VkRenderPass GetRenderPass() const { return renderPass; }
};

#endif //VKGPURENDERPASS_H
