//
// Created by 1234 on 2025/3/6.
//

#include "EffectEngine.h"

#include <iostream>
#include <ostream>
#include <vulkan/vk_enum_string_helper.h>

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
    return true;
}

void EffectEngine::Process(const ImageInfo &input,
                           const ImageInfo &output,
                           const std::shared_ptr<GrayFilter> &filter) const {
    if (input.width != output.width || input.height != output.height || input.channels != output.channels) {
        std::cerr << "Input and output must be same size!" << std::endl;
        return;
    }
    const VkDeviceSize bufferSize = input.width * input.height * input.channels;
    VkBuffer inputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory inputStorageBufferMemory = VK_NULL_HANDLE;
    std::vector<uint32_t> queueFamilyIndices;
    queueFamilyIndices.push_back(0);
    const VkPhysicalDeviceMemoryProperties memoryProperties = gpuCtx->GetMemoryProperties();
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

    void *data = nullptr;
    ret = vkMapMemory(gpuCtx->GetCurrentDevice(), inputStorageBufferMemory, 0, bufferSize, 0, &data);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to map input storage buffer memory, err=" << string_VkResult(ret) << std::endl;
        return;
    }
    memcpy(data, input.data, bufferSize);
    vkUnmapMemory(gpuCtx->GetCurrentDevice(), inputStorageBufferMemory);


    ret = filter->Apply(gpuCtx, bufferSize, input.width, input.height, inputStorageBuffer, outputStorageBuffer);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to apply filter!" << std::endl;
        return;
    }

    ret = vkMapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory, 0, bufferSize, 0, &data);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to map output storage buffer memory, err=" << string_VkResult(ret) << std::endl;
        return;
    }
    memcpy(output.data, data, bufferSize);
    vkUnmapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory);
}

void EffectEngine::Process(const char *inputFilePath,
                           const char *outputFilePath,
                           const std::shared_ptr<GrayFilter> &filter) const {
}
