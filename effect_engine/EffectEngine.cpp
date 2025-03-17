//
// Created by 1234 on 2025/3/6.
//

#include "EffectEngine.h"

#include <iostream>
#include <ostream>
#ifdef Q_OS_OPENHARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "utils/ImageUtils.h"
#include "gpu/VkGPUHelper.h"
#include "log/Log.h"
#include "utils/TimeUtils.h"

bool EffectEngine::Init() {
    std::vector<const char *> requiredExtensions;
    this->gpuCtx = std::make_shared<VkGPUContext>(requiredExtensions);
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_validation");
    const VkResult result = this->gpuCtx->Init();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to initialize Vulkan GPU context!" << std::endl;
        return false;
    }
    return true;
}

#include <fstream>

VkResult EffectEngine::Process(VkBuffer *inputStorageBuffer,
                               VkDeviceMemory *inputStorageBufferMemory,
                               const uint32_t inputWidth,
                               const uint32_t inputHeight,
                               const uint32_t outputWidth,
                               const uint32_t outputHeight,
                               const uint32_t channels,
                               const void *uploadData,
                               VkBuffer *outputStorageBuffer,
                               VkDeviceMemory *outputStorageBufferMemory,
                               const std::shared_ptr<IFilter> &filter) const {
    const VkDeviceSize inputBufferSize = inputWidth * inputHeight * channels;
    const VkDeviceSize outputBufferSize = outputWidth * outputHeight * channels;

    std::vector<uint32_t> queueFamilyIndices;
    queueFamilyIndices.push_back(0);
    const VkPhysicalDeviceMemoryProperties memoryProperties = gpuCtx->GetMemoryProperties();
    const uint64_t imageBufferPrepareStart = TimeUtils::GetCurrentMonoMs();
    VkResult ret = VkGPUHelper::CreateStorageBufferAndUploadData(gpuCtx->GetCurrentDevice(),
                                                                 queueFamilyIndices,
                                                                 &memoryProperties,
                                                                 inputBufferSize,
                                                                 inputStorageBuffer,
                                                                 inputStorageBufferMemory,
                                                                 uploadData);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create Vulkan buffer memory object, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    const uint64_t imageBufferPrepareEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "Image Buffer Prepare And Upload Time: " << imageBufferPrepareEnd - imageBufferPrepareStart << "ms" <<
            std::endl;

    ret = VkGPUHelper::CreateStorageBufferAndBindMem(gpuCtx->GetCurrentDevice(),
                                                     outputBufferSize,
                                                     queueFamilyIndices,
                                                     &memoryProperties,
                                                     outputStorageBuffer,
                                                     outputStorageBufferMemory);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create output storage buffer!" << std::endl;
        return ret;
    }

    const uint64_t gpuProcessTimeStart = TimeUtils::GetCurrentMonoMs();
    ret = filter->Apply(gpuCtx, inputBufferSize, inputWidth, inputHeight, *inputStorageBuffer, *outputStorageBuffer);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to apply filter!" << std::endl;
        return ret;
    }
    const uint64_t gpuProcessTimeEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "GPU Process Time: " << gpuProcessTimeEnd - gpuProcessTimeStart << "ms" << std::endl;
    filter->Destroy();

    return ret;
}

void EffectEngine::Process(const ImageInfo &input,
                           const ImageInfo &output,
                           const std::shared_ptr<IFilter> &filter) const {
    if (input.channels != output.channels) {
        std::cerr << "Input and output channel must be same size!" << std::endl;
        return;
    }
    if (input.width != output.width || input.height != output.height) {
        Logger() << "Scale from (" << input.width << "," << input.height
                << ") to (" << output.width << "," << output.height << ")"
                << std::endl;
    }
    const VkDeviceSize outputBufferSize = output.width * output.height * output.channels;
    VkBuffer inputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory inputStorageBufferMemory = VK_NULL_HANDLE;
    VkBuffer outputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory outputStorageBufferMemory = VK_NULL_HANDLE;

    VkResult ret = Process(&inputStorageBuffer,
                           &inputStorageBufferMemory,
                           input.width,
                           input.height,
                           output.width,
                           output.height,
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
    ret = vkMapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory, 0, outputBufferSize, 0, &data);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to map output storage buffer memory, err=" << string_VkResult(ret) << std::endl;
        return;
    }
    memcpy(output.data, data, outputBufferSize);
    vkUnmapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory);
    const uint64_t imageDownloadEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "Image Download Time: " << imageDownloadEnd - imageDownloadStart << "ms" << std::endl;

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
    Logger() << "Image width= " << imageWidth << ", height=" << imageHeight << ", channels=" << channels << std::endl;
    const VkDeviceSize outputBufferSize = imageWidth * imageHeight * channels;

    VkBuffer inputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory inputStorageBufferMemory = VK_NULL_HANDLE;
    VkBuffer outputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory outputStorageBufferMemory = VK_NULL_HANDLE;

    VkResult ret = Process(&inputStorageBuffer,
                           &inputStorageBufferMemory,
                           imageWidth,
                           imageHeight,
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
    ret = vkMapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory, 0, outputBufferSize, 0, &data);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to map output storage buffer memory, err=" << string_VkResult(ret) << std::endl;
        return;
    }

    ImageUtils::WritePngFile(outputFilePath, imageWidth, imageHeight, channels, data);
    vkUnmapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory);

    vkFreeMemory(gpuCtx->GetCurrentDevice(), inputStorageBufferMemory, nullptr);
    vkDestroyBuffer(gpuCtx->GetCurrentDevice(), inputStorageBuffer, nullptr);
    vkFreeMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory, nullptr);
    vkDestroyBuffer(gpuCtx->GetCurrentDevice(), outputStorageBuffer, nullptr);
}

void EffectEngine::Process(const char *inputFilePath,
                           const char *outputFilePath,
                           const uint32_t newWidth,
                           const uint32_t newHeight,
                           const std::shared_ptr<IFilter> &filter) const {
    uint32_t imageWidth = 0, imageHeight = 0, channels = 0;
    std::vector<char> inputFileData =
            ImageUtils::ReadPngFile(inputFilePath, &imageWidth, &imageHeight, &channels);
    if (inputFileData.empty()) {
        std::cerr << "Failed to read input file!" << std::endl;
        return;
    }
    Logger() << "Image width= " << imageWidth << ", height=" << imageHeight << ", channels=" << channels << std::endl;
    const VkDeviceSize outputBufferSize = newWidth * newHeight * channels;

    VkBuffer inputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory inputStorageBufferMemory = VK_NULL_HANDLE;
    VkBuffer outputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory outputStorageBufferMemory = VK_NULL_HANDLE;

    VkResult ret = Process(&inputStorageBuffer,
                           &inputStorageBufferMemory,
                           imageWidth,
                           imageHeight,
                           newWidth,
                           newHeight,
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
    ret = vkMapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory, 0, outputBufferSize, 0, &data);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to map output storage buffer memory, err=" << string_VkResult(ret) << std::endl;
        return;
    }

    ImageUtils::WritePngFile(outputFilePath, newWidth, newHeight, channels, data);
    vkUnmapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory);

    vkFreeMemory(gpuCtx->GetCurrentDevice(), inputStorageBufferMemory, nullptr);
    vkDestroyBuffer(gpuCtx->GetCurrentDevice(), inputStorageBuffer, nullptr);
    vkFreeMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory, nullptr);
    vkDestroyBuffer(gpuCtx->GetCurrentDevice(), outputStorageBuffer, nullptr);
}

void EffectEngine::Process(const char *baseFilePath,
                           const char *blendFilePath,
                           const uint32_t posX,
                           const uint32_t posY,
                           const char *outputFilePath,
                           const std::shared_ptr<IBlender> &blender) const {
    uint32_t baseImageWidth = 0, baseImageHeight = 0, baseImageChannels = 0;
    std::vector<char> baseImageFileData =
            ImageUtils::ReadPngFile(baseFilePath, &baseImageWidth, &baseImageHeight, &baseImageChannels);
    if (baseImageFileData.empty()) {
        std::cerr << "Failed to read input base file!" << std::endl;
        return;
    }
    Logger() << "Base Image width= " << baseImageWidth << ", height=" << baseImageHeight << ", channels=" <<
            baseImageChannels << std::endl;

    uint32_t blendImageWidth = 0, blendImageHeight = 0, blendImageChannels = 0;
    std::vector<char> blendImageFileData =
            ImageUtils::ReadPngFile(blendFilePath, &blendImageWidth, &blendImageHeight, &blendImageChannels);
    if (blendImageFileData.empty()) {
        std::cerr << "Failed to read input blend file!" << std::endl;
        return;
    }
    Logger() << "Blend Image width= " << blendImageWidth << ", height=" << blendImageHeight << ", channels=" <<
            blendImageChannels << std::endl;

    const VkDeviceSize outputBufferSize = baseImageWidth * baseImageHeight * baseImageChannels;
    VkBuffer baseStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory baseStorageBufferMemory = VK_NULL_HANDLE;
    VkBuffer blendStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory blendStorageBufferMemory = VK_NULL_HANDLE;
    VkBuffer outputStorageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory outputStorageBufferMemory = VK_NULL_HANDLE;

    std::vector<uint32_t> queueFamilyIndices;
    queueFamilyIndices.push_back(0);
    const VkPhysicalDeviceMemoryProperties memoryProperties = gpuCtx->GetMemoryProperties();

    const VkDeviceSize baseBufferSize = baseImageWidth * baseImageHeight * baseImageChannels;
    const uint64_t baseImageBufferPrepareStart = TimeUtils::GetCurrentMonoMs();
    VkResult ret = VkGPUHelper::CreateStorageBufferAndUploadData(gpuCtx->GetCurrentDevice(),
                                                                 queueFamilyIndices,
                                                                 &memoryProperties,
                                                                 baseBufferSize,
                                                                 &baseStorageBuffer,
                                                                 &baseStorageBufferMemory,
                                                                 baseImageFileData.data());
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create Vulkan base buffer memory object, err=" << string_VkResult(ret) << std::endl;
        return;
    }
    const uint64_t baseImageBufferPrepareEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "Base Image Buffer Prepare And Upload Time: " << baseImageBufferPrepareEnd -
            baseImageBufferPrepareStart << "ms" <<
            std::endl;
    baseImageFileData.clear();
    baseImageFileData.resize(0);

    const VkDeviceSize blendBufferSize = blendImageWidth * blendImageHeight * blendImageChannels;
    const uint64_t blendImageBufferPrepareStart = TimeUtils::GetCurrentMonoMs();
    ret = VkGPUHelper::CreateStorageBufferAndUploadData(gpuCtx->GetCurrentDevice(),
                                                        queueFamilyIndices,
                                                        &memoryProperties,
                                                        blendBufferSize,
                                                        &blendStorageBuffer,
                                                        &blendStorageBufferMemory,
                                                        blendImageFileData.data());
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create Vulkan buffer memory object, err=" << string_VkResult(ret) << std::endl;
        return;
    }
    const uint64_t blendImageBufferPrepareEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "Blend Image Buffer Prepare And Upload Time: " << blendImageBufferPrepareEnd -
            blendImageBufferPrepareStart << "ms" <<
            std::endl << std::endl;

    blendImageFileData.clear();
    blendImageFileData.resize(0);

    ret = VkGPUHelper::CreateStorageBufferAndBindMem(gpuCtx->GetCurrentDevice(),
                                                     outputBufferSize,
                                                     queueFamilyIndices,
                                                     &memoryProperties,
                                                     &outputStorageBuffer,
                                                     &outputStorageBufferMemory);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create output storage buffer!" << std::endl;
        return;
    }

    const uint64_t gpuProcessTimeStart = TimeUtils::GetCurrentMonoMs();
    BlendImageInfo baseImageInfo;
    baseImageInfo.width = baseImageWidth;
    baseImageInfo.height = baseImageHeight;
    baseImageInfo.channels = baseImageChannels;
    baseImageInfo.bufferSize = baseBufferSize;
    baseImageInfo.posX = 0;
    baseImageInfo.posY = 0;
    baseImageInfo.storageBuffer = baseStorageBuffer;
    BlendImageInfo blendImageInfo;
    blendImageInfo.width = blendImageWidth;
    blendImageInfo.height = blendImageHeight;
    blendImageInfo.channels = baseImageChannels;
    blendImageInfo.bufferSize = blendBufferSize;
    blendImageInfo.posX = posX;
    blendImageInfo.posY = posY;
    blendImageInfo.storageBuffer = blendStorageBuffer;
    ret = blender->Apply(gpuCtx, baseImageInfo, blendImageInfo, outputStorageBuffer);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to apply blender!" << std::endl;
        return;
    }
    const uint64_t gpuProcessTimeEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "GPU Process Time: " << gpuProcessTimeEnd - gpuProcessTimeStart << "ms" << std::endl;

    void *data = nullptr;
    ret = vkMapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory, 0, outputBufferSize, 0, &data);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to map output storage buffer memory, err=" << string_VkResult(ret) << std::endl;
        return;
    }


    ImageUtils::WritePngFile(outputFilePath, baseImageWidth, baseImageHeight, baseImageChannels, data);
    vkUnmapMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory);

    vkFreeMemory(gpuCtx->GetCurrentDevice(), baseStorageBufferMemory, nullptr);
    vkDestroyBuffer(gpuCtx->GetCurrentDevice(), baseStorageBuffer, nullptr);
    vkFreeMemory(gpuCtx->GetCurrentDevice(), blendStorageBufferMemory, nullptr);
    vkDestroyBuffer(gpuCtx->GetCurrentDevice(), blendStorageBuffer, nullptr);
    vkFreeMemory(gpuCtx->GetCurrentDevice(), outputStorageBufferMemory, nullptr);
    vkDestroyBuffer(gpuCtx->GetCurrentDevice(), outputStorageBuffer, nullptr);
}
