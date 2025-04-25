//
// Created by neo on 25-4-14.
//

#include "VkGPUSwapChain.h"

#ifdef OS_OPEN_HARMONY
#include <gpu_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "log/Log.h"

VkGPUSwapChain::VkGPUSwapChain(const std::shared_ptr<VkGPUContext> &gpuCtx) {
    this->gpuCtx = gpuCtx;
}

VkResult VkGPUSwapChain::CreateSwapChain(const VkSurfaceKHR surface,
                                         const uint32_t width,
                                         const uint32_t height,
                                         const std::vector<uint32_t> &queueFamilies) {
    if (gpuCtx == nullptr) {
        Logger() << "gpuCtx is null!" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->gpuCtx->GetPhysicalDevice(),
                                              surface,
                                              &capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(this->gpuCtx->GetPhysicalDevice(),
                                         surface,
                                         &surfaceFormatCount,
                                         nullptr);

    vkGetPhysicalDeviceSurfacePresentModesKHR(this->gpuCtx->GetPhysicalDevice(),
                                              surface,
                                              &presentModeCount,
                                              nullptr);
    presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(this->gpuCtx->GetPhysicalDevice(),
                                              surface,
                                              &presentModeCount,
                                              presentModes.data());
    for (uint32_t i = 0; i < presentModeCount; i++) {
        Logger() << "presentMode: " << string_VkPresentModeKHR(presentModes[i]) << std::endl;
    }

    surfaceFormats.resize(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(this->gpuCtx->GetPhysicalDevice(),
                                         surface,
                                         &surfaceFormatCount,
                                         surfaceFormats.data());

    for (uint32_t i = 0; i < surfaceFormatCount; i++) {
        Logger() << "support surface format = "
                << string_VkFormat(surfaceFormats[i].format) << ", color space ="
                << string_VkColorSpaceKHR(surfaceFormats[i].colorSpace)
                << std::endl;
    }

    VkExtent2D imageExtent;
    imageExtent.width = width;
    imageExtent.height = height;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.flags = 0;
    createInfo.pNext = nullptr;
    createInfo.surface = surface;
    createInfo.minImageCount = capabilities.minImageCount;
    createInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    createInfo.imageExtent = imageExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    VkResult result = vkCreateSwapchainKHR(this->gpuCtx->GetCurrentDevice(),
                                           &createInfo,
                                           nullptr,
                                           &this->swapChain);
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan swap chain, err=" << string_VkResult(result) << std::endl;
        return result;
    }

    vkGetSwapchainImagesKHR(this->gpuCtx->GetCurrentDevice(),
                            this->swapChain,
                            &this->swapChainImageCount,
                            nullptr);
    Logger() << "swap chain image count: " << this->swapChainImageCount << std::endl;
    this->swapChainImages.resize(this->swapChainImageCount);
    result = vkGetSwapchainImagesKHR(this->gpuCtx->GetCurrentDevice(),
                                     this->swapChain,
                                     &this->swapChainImageCount,
                                     this->swapChainImages.data());
    if (result != VK_SUCCESS) {
        Logger() << "Failed to get swap chain images, err=" << string_VkResult(result) << std::endl;
    }
    return result;
}

void VkGPUSwapChain::Destroy() {
    if (swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(this->gpuCtx->GetCurrentDevice(), swapChain, nullptr);
        this->swapChain = VK_NULL_HANDLE;
    }
}
