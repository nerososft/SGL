//
// Created by neo on 25-4-14.
//

#ifndef VKGPUSWAPCHAIN_H
#define VKGPUSWAPCHAIN_H
#include <vector>

#include "VkGPUFramebuffer.h"

class VkGPUSwapChain {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::vector<std::shared_ptr<VkGPUFramebuffer> > frameBuffers;
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;

public:
    explicit VkGPUSwapChain(const std::shared_ptr<VkGPUContext> &gpuCtx);

    VkResult CreateSwapChain(VkSurfaceKHR surface, const std::vector<uint32_t> &queueFamilies);

    void Destroy();

    ~VkGPUSwapChain() = default;
};

#endif //VKGPUSWAPCHAIN_H
