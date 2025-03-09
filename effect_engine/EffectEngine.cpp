//
// Created by 1234 on 2025/3/6.
//

#include "EffectEngine.h"

#include <iostream>
#include <ostream>

#include "gpu/VkGPUHelper.h"

bool EffectEngine::Init() {
    std::vector<const char *> requiredExtensions;
    std::vector<const char *> enabledExtensions;
    this->gpuCtx = std::make_shared<VkGPUContext>(requiredExtensions);
    const VkResult result = this->gpuCtx->Init();
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to initialize Vulkan GPU context!" << std::endl;
        return false;
    }
}

void EffectEngine::Process(const char *inputFilePath, const char *outputFilePath,
                           const std::shared_ptr<GrayFilter> &filter) {
    uint32_t width = 0, height = 0, channels = 0;
    VkDeviceSize bufferSize = width * height * channels;
    VkBuffer inputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory inputStorageBufferMemory = VK_NULL_HANDLE;
    std::vector<uint32_t> queueFamilyIndices;
    queueFamilyIndices.push_back(0);
    VkPhysicalDeviceMemoryProperties memoryProperties = gpuCtx->GetMemoryProperties();
    VkResult ret = VkGPUHelper::CreateStorageBufferAndBindMem(gpuCtx->GetCurrentDevice(),
                                                              bufferSize,
                                                              queueFamilyIndices,
                                                              &memoryProperties,
                                                              &inputStorageBuffer,
                                                              &inputStorageBufferMemory);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to create input storage buffer!" << std::endl;
        return;
    }

    VkBuffer outputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory outputStorageBufferMemory = VK_NULL_HANDLE;
    ret = VkGPUHelper::CreateStorageBufferAndBindMem(gpuCtx->GetCurrentDevice(),
                                                     bufferSize,
                                                     queueFamilyIndices,
                                                     &memoryProperties,
                                                     &outputStorageBuffer,
                                                     &outputStorageBufferMemory);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to create output storage buffer!" << std::endl;
        return;
    }
}
