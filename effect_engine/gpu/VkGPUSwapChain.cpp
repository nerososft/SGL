//
// Created by neo on 25-4-14.
//

#include "VkGPUSwapChain.h"

#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "effect_engine/log/Log.h"

VkGPUSwapChain::VkGPUSwapChain(const std::shared_ptr<VkGPUContext> &gpuCtx) {
    this->gpuCtx = gpuCtx;
}

VkResult VkGPUSwapChain::CreateSwapChain(const VkSurfaceKHR surface, const std::vector<uint32_t> &queueFamilies) {
    if (gpuCtx == nullptr) {
        Logger() << "gpuctx is null!" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    VkResult result = VK_SUCCESS;

    // TODO: fixme
    VkSwapchainCreateInfoKHR swapChainInfo = {};
    swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainInfo.flags = 0;
    swapChainInfo.pNext = nullptr;
    swapChainInfo.surface = surface;
    swapChainInfo.minImageCount = 1;
    swapChainInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
    swapChainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapChainInfo.imageExtent = {1280, 768};
    swapChainInfo.imageArrayLayers = 1;
    swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainInfo.queueFamilyIndexCount = queueFamilies.size();
    swapChainInfo.pQueueFamilyIndices = queueFamilies.data();
    swapChainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapChainInfo.clipped = VK_FALSE;
    swapChainInfo.oldSwapchain = VK_NULL_HANDLE;

    result = vkCreateSwapchainKHR(this->gpuCtx->GetCurrentDevice(), &swapChainInfo, nullptr, &swapChain);
    if (result != VK_SUCCESS) {
        Logger() << "failed to create swap chain, err=" << string_VkResult(result) << std::endl;
        return result;
    }
    return result;
}

void VkGPUSwapChain::Destroy() {
    if (swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(this->gpuCtx->GetCurrentDevice(), swapChain, nullptr);
        this->swapChain = VK_NULL_HANDLE;
    }
}
