//
// Created by 1234 on 2025/3/6.
//

#include "EffectEngine.h"

#include <iostream>
#include <ostream>
#include <vulkan/vk_enum_string_helper.h>

#include "demo/utils/ImageUtils.h"
#include "gpu/VkGPUHelper.h"
#include "utils/TimeUtils.h"

bool EffectEngine::Init() {
    std::vector<const char *> requiredExtensions;
    this->gpuCtx = std::make_shared<VkGPUContext>(requiredExtensions);
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_validation");
    const VkResult result = this->gpuCtx->Init();
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to initialize Vulkan GPU context!" << std::endl;
        return false;
    }
    return true;
}

VkResult EffectEngine::Process(VkBuffer *inputStorageBuffer,
                               VkDeviceMemory *inputStorageBufferMemory,
                               const uint32_t width,
                               const uint32_t height,
                               const uint32_t channels,
                               const void *uploadData,
                               VkBuffer *outputStorageBuffer,
                               VkDeviceMemory *outputStorageBufferMemory,
                               const std::shared_ptr<IFilter> &filter) const {
    const VkDeviceSize bufferSize = width * height * channels;

    std::vector<uint32_t> queueFamilyIndices;
    queueFamilyIndices.push_back(0);
    const VkPhysicalDeviceMemoryProperties memoryProperties = gpuCtx->GetMemoryProperties();
    const uint64_t imageBufferPrepareStart = TimeUtils::GetCurrentMonoMs();
    VkResult ret = VkGPUHelper::CreateStorageBufferAndBindMem(gpuCtx->GetCurrentDevice(),
                                                              bufferSize,
                                                              queueFamilyIndices,
                                                              &memoryProperties,
                                                              inputStorageBuffer,
                                                              inputStorageBufferMemory);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to create input storage buffer!" << std::endl;
        return ret;
    }

    ret = VkGPUHelper::CreateStorageBufferAndBindMem(gpuCtx->GetCurrentDevice(),
                                                     bufferSize,
                                                     queueFamilyIndices,
                                                     &memoryProperties,
                                                     outputStorageBuffer,
                                                     outputStorageBufferMemory);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to create output storage buffer!" << std::endl;
        return ret;
    }
    const uint64_t imageBufferPrepareEnd = TimeUtils::GetCurrentMonoMs();
    std::cout << "Image Buffer Prepare Time: " << imageBufferPrepareEnd - imageBufferPrepareStart << "ms" << std::endl;

    const uint64_t imageUploadStart = TimeUtils::GetCurrentMonoMs();
    void *data = nullptr;
    ret = vkMapMemory(gpuCtx->GetCurrentDevice(), *inputStorageBufferMemory, 0, bufferSize, 0, &data);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to map input storage buffer memory, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    memcpy(data, uploadData, bufferSize);
    vkUnmapMemory(gpuCtx->GetCurrentDevice(), *inputStorageBufferMemory);
    const uint64_t imageUploadEnd = TimeUtils::GetCurrentMonoMs();
    std::cout << "Image Upload Time: " << imageUploadEnd - imageUploadStart << "ms" << std::endl;

    const uint64_t gpuProcessTimeStart = TimeUtils::GetCurrentMonoMs();
    ret = filter->Apply(gpuCtx, bufferSize, width, height, *inputStorageBuffer, *outputStorageBuffer);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to apply filter!" << std::endl;
        return ret;
    }
    const uint64_t gpuProcessTimeEnd = TimeUtils::GetCurrentMonoMs();
    std::cout << "GPU Process Time: " << gpuProcessTimeEnd - gpuProcessTimeStart << "ms" << std::endl;
    filter->Destroy();
    return ret;
}

void EffectEngine::Process(const ImageInfo &input,
                           const ImageInfo &output,
                           const std::shared_ptr<IFilter> &filter) const {
    if (input.width != output.width || input.height != output.height || input.channels != output.channels) {
        std::cerr << "Input and output must be same size!" << std::endl;
        return;
    }
    const VkDeviceSize bufferSize = input.width * input.height * input.channels;
    VkBuffer inputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory inputStorageBufferMemory = VK_NULL_HANDLE;
    VkBuffer outputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory outputStorageBufferMemory = VK_NULL_HANDLE;

    VkResult ret = Process(&inputStorageBuffer,
                           &inputStorageBufferMemory,
                           input.width,
                           input.height,
                           input.channels,
                           input.data,
                           &outputStorageBuffer,
                           &outputStorageBufferMemory,
                           filter);
    if (ret != VK_SUCCESS) {
        std::cerr << "Failed to process input storage buffer, err=" << string_VkResult(ret) << std::endl;
        return;
    }

    const uint64_t imageDownloadStart = TimeUtils::GetCurrentMonoMs();
    void *data = nullptr;
    ret = vkMapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory, 0, bufferSize, 0, &data);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to map output storage buffer memory, err=" << string_VkResult(ret) << std::endl;
        return;
    }
    memcpy(output.data, data, bufferSize);
    vkUnmapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory);
    const uint64_t imageDownloadEnd = TimeUtils::GetCurrentMonoMs();
    std::cout << "Image Download Time: " << imageDownloadEnd - imageDownloadStart << "ms" << std::endl;

    vkFreeMemory(gpuCtx->GetCurrentDevice(), inputStorageBufferMemory, nullptr);
    vkDestroyBuffer(gpuCtx->GetCurrentDevice(), inputStorageBuffer, nullptr);
    vkFreeMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory, nullptr);
    vkDestroyBuffer(gpuCtx->GetCurrentDevice(), outputStorageBuffer, nullptr);
}

void EffectEngine::Process(const char *inputFilePath,
                           const char *outputFilePath,
                           const std::shared_ptr<IFilter> &filter) const {
    uint32_t imageWidth = 0, imageHeight = 0, channels = 0;
    std::vector<char> inputFileData =
            ImageUtils::ReadPngFile(inputFilePath, &imageWidth, &imageHeight, &channels);
    if (inputFileData.empty()) {
        std::cerr << "Failed to read input file!" << std::endl;
        return;
    }
    std::cout << "Image width= " << imageWidth << ", height=" << imageHeight << ", channels=" << channels << std::endl;
    const VkDeviceSize bufferSize = imageWidth * imageHeight * channels;

    VkBuffer inputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory inputStorageBufferMemory = VK_NULL_HANDLE;
    VkBuffer outputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory outputStorageBufferMemory = VK_NULL_HANDLE;

    VkResult ret = Process(&inputStorageBuffer,
                           &inputStorageBufferMemory,
                           imageWidth,
                           imageHeight,
                           channels,
                           inputFileData.data(),
                           &outputStorageBuffer,
                           &outputStorageBufferMemory,
                           filter);
    if (ret != VK_SUCCESS) {
        std::cerr << "Failed to process input storage buffer, err=" << string_VkResult(ret) << std::endl;
        return;
    }
    inputFileData.clear();
    inputFileData.resize(0);

    void *data = nullptr;
    ret = vkMapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory, 0, bufferSize, 0, &data);
    if (ret != VK_SUCCESS) {
        std::cout << "Failed to map output storage buffer memory, err=" << string_VkResult(ret) << std::endl;
        return;
    }

    ImageUtils::WritePngFile(outputFilePath, imageWidth, imageHeight, channels, data);
    vkUnmapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory);

    vkFreeMemory(gpuCtx->GetCurrentDevice(), inputStorageBufferMemory, nullptr);
    vkDestroyBuffer(gpuCtx->GetCurrentDevice(), inputStorageBuffer, nullptr);
    vkFreeMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory, nullptr);
    vkDestroyBuffer(gpuCtx->GetCurrentDevice(), outputStorageBuffer, nullptr);
}
