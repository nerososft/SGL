//
// Created by neo on 25-4-14.
//

#ifndef VKGPUSWAPCHAIN_H
#define VKGPUSWAPCHAIN_H
#include <vector>

#include "VkGPUFramebuffer.h"

class VkGPUSwapChain {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;

    VkSurfaceCapabilitiesKHR capabilities{};
    uint32_t surfaceFormatCount = 0;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;

    uint32_t presentModeCount = 0;
    std::vector<VkPresentModeKHR> presentModes;

    uint32_t swapChainImageCount = 0;
    std::vector<VkImage> swapChainImages;

    uint32_t width = 0;
    uint32_t height = 0;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;

    std::vector<std::shared_ptr<VkGPUFramebuffer> > frameBuffers;

public:
    explicit VkGPUSwapChain(const std::shared_ptr<VkGPUContext> &gpuCtx);

    VkResult CreateSwapChain(VkSurfaceKHR surface,
                             uint32_t width,
                             uint32_t height,
                             const std::vector<uint32_t> &queueFamilies);


    [[nodiscard]] VkSwapchainKHR GetSwapChain() const { return swapChain; }

    [[nodiscard]] VkImage GetSwapChainImg(const uint32_t index = 0) const { return swapChainImages[index]; }

    void Destroy();

    ~VkGPUSwapChain() = default;
};

#endif //VKGPUSWAPCHAIN_H
