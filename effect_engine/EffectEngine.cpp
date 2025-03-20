//
// Created by 1234 on 2025/3/6.
//

#include "EffectEngine.h"

#include <iostream>
#include <ostream>
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "config.h"
#include "gpu/VkGPUBuffer.h"
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
    Logger() << Logger::INFO << "Initialized EffectEngine, version: " << VERSION << std::endl;
    return true;
}

#include <fstream>

VkResult EffectEngine::Process(const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                               const uint32_t inputWidth,
                               const uint32_t inputHeight,
                               const uint32_t outputWidth,
                               const uint32_t outputHeight,
                               const uint32_t channels,
                               const void *uploadData,
                               const std::shared_ptr<VkGPUBuffer> &outputBuffer,
                               const std::shared_ptr<IFilter> &filter) const {
    const VkDeviceSize inputBufferSize = inputWidth * inputHeight * channels;
    const VkDeviceSize outputBufferSize = outputWidth * outputHeight * channels;
    const uint64_t imageBufferPrepareStart = TimeUtils::GetCurrentMonoMs();
    VkResult ret = inputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, inputBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to allocate input GPU buffer!" << std::endl;
        return ret;
    }
    ret = inputBuffer->UploadData(uploadData, inputBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to upload data!" << std::endl;
        return ret;
    }
    const uint64_t imageBufferPrepareEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "Image Buffer Prepare And Upload Time: " << imageBufferPrepareEnd - imageBufferPrepareStart << "ms" <<
            std::endl;

    ret = outputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, outputBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to allocate output GPU buffer!" << std::endl;
        return ret;
    }

    const uint64_t gpuProcessTimeStart = TimeUtils::GetCurrentMonoMs();
    ret = filter->Apply(gpuCtx, inputBufferSize, inputWidth, inputHeight, inputBuffer->GetBuffer(),
                        outputBuffer->GetBuffer());
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
        Logger() << "Input and output channel must be same size!" << std::endl;
        return;
    }
    if (input.width != output.width || input.height != output.height) {
        Logger() << "Scale from (" << input.width << "," << input.height
                << ") to (" << output.width << "," << output.height << ")"
                << std::endl;
    }
    const VkDeviceSize outputBufferSize = output.width * output.height * output.channels;
    const auto inputStorageBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    const auto outputStorageBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);

    const VkResult ret = Process(inputStorageBuffer,
                                 input.width,
                                 input.height,
                                 output.width,
                                 output.height,
                                 input.channels,
                                 input.data,
                                 outputStorageBuffer,
                                 filter);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to process input storage buffer, err=" << string_VkResult(ret) << std::endl;
        return;
    }

    const uint64_t imageDownloadStart = TimeUtils::GetCurrentMonoMs();
    outputStorageBuffer->DownloadData(output.data, outputBufferSize);
    const uint64_t imageDownloadEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "Image Download Time: " << imageDownloadEnd - imageDownloadStart << "ms" << std::endl;

    inputStorageBuffer->Destroy();
    outputStorageBuffer->Destroy();
}

void EffectEngine::Process(const char *inputFilePath,
                           const char *outputFilePath,
                           const std::shared_ptr<IFilter> &filter) const {
    uint32_t imageWidth = 0, imageHeight = 0, channels = 0;
    std::vector<char> inputFileData = ImageUtils::ReadPngFile(inputFilePath, &imageWidth, &imageHeight, &channels);

    if (inputFileData.empty()) {
        Logger() << "Failed to read input file!" << std::endl;
        return;
    }
    Logger() << "Image width= " << imageWidth << ", height=" << imageHeight << ", channels=" << channels << std::endl;
    const VkDeviceSize outputBufferSize = imageWidth * imageHeight * channels;

    const auto inputStorageBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    const auto outputStorageBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);

    VkResult ret = Process(inputStorageBuffer,
                           imageWidth,
                           imageHeight,
                           imageWidth,
                           imageHeight,
                           channels,
                           inputFileData.data(),
                           outputStorageBuffer,
                           filter);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to process input storage buffer, err=" << string_VkResult(ret) << std::endl;
        return;
    }
    inputFileData.clear();
    inputFileData.resize(0);

    ret = outputStorageBuffer->MapBuffer(outputBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to map output buffer!" << std::endl;
        inputStorageBuffer->Destroy();
        outputStorageBuffer->Destroy();
        return;
    }

    ImageUtils::WritePngFile(outputFilePath, imageWidth, imageHeight, channels, outputStorageBuffer->GetMappedAddr());
    inputStorageBuffer->Destroy();
    outputStorageBuffer->Destroy();
}

void EffectEngine::Process(const char *inputFilePath,
                           const char *outputFilePath,
                           const uint32_t newWidth,
                           const uint32_t newHeight,
                           const std::shared_ptr<IFilter> &filter) const {
    uint32_t imageWidth = 0, imageHeight = 0, channels = 0;
    std::vector<char> inputFileData = ImageUtils::ReadPngFile(inputFilePath, &imageWidth, &imageHeight, &channels);
    if (inputFileData.empty()) {
        Logger() << "Failed to read input file!" << std::endl;
        return;
    }
    Logger() << "Image width= " << imageWidth << ", height=" << imageHeight << ", channels=" << channels << std::endl;
    const VkDeviceSize outputBufferSize = newWidth * newHeight * channels;

    const auto inputStorageBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    const auto outputStorageBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);

    VkResult ret = Process(inputStorageBuffer,
                           imageWidth,
                           imageHeight,
                           newWidth,
                           newHeight,
                           channels,
                           inputFileData.data(),
                           outputStorageBuffer,
                           filter);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to process input storage buffer, err=" << string_VkResult(ret) << std::endl;
        return;
    }
    inputFileData.clear();
    inputFileData.resize(0);

    ret = outputStorageBuffer->MapBuffer(outputBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to map output buffer!" << std::endl;
        inputStorageBuffer->Destroy();
        outputStorageBuffer->Destroy();
        return;
    }

    ImageUtils::WritePngFile(outputFilePath, imageWidth, imageHeight, channels, outputStorageBuffer->GetMappedAddr());
    inputStorageBuffer->Destroy();
    outputStorageBuffer->Destroy();
}

void EffectEngine::Process(const char *baseFilePath,
                           const char *blendFilePath,
                           const uint32_t posX,
                           const uint32_t posY,
                           const char *outputFilePath,
                           const std::shared_ptr<IBlender> &blender) const {
    uint32_t baseImageWidth = 0, baseImageHeight = 0, baseImageChannels = 0;
    std::vector<char> baseImageFileData = ImageUtils::ReadPngFile(baseFilePath, &baseImageWidth, &baseImageHeight,
                                                                  &baseImageChannels);
    if (baseImageFileData.empty()) {
        Logger() << "Failed to read input base file!" << std::endl;
        return;
    }
    Logger() << "Base Image width= " << baseImageWidth << ", height=" << baseImageHeight << ", channels=" <<
            baseImageChannels << std::endl;

    uint32_t blendImageWidth = 0, blendImageHeight = 0, blendImageChannels = 0;
    std::vector<char> blendImageFileData = ImageUtils::ReadPngFile(blendFilePath, &blendImageWidth, &blendImageHeight,
                                                                   &blendImageChannels);
    if (blendImageFileData.empty()) {
        Logger() << "Failed to read input blend file!" << std::endl;
        return;
    }
    Logger() << "Blend Image width= " << blendImageWidth << ", height=" << blendImageHeight << ", channels=" <<
            blendImageChannels << std::endl;

    const VkDeviceSize outputBufferSize = baseImageWidth * baseImageHeight * baseImageChannels;
    const auto baseStorageBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    const auto blendStorageBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    const auto outputStorageBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);

    const uint64_t baseImageBufferPrepareStart = TimeUtils::GetCurrentMonoMs();
    const VkDeviceSize baseBufferSize = baseImageWidth * baseImageHeight * baseImageChannels;
    VkResult ret = baseStorageBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, baseBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to allocate base storage buffer!" << std::endl;
        return;
    }
    ret = baseStorageBuffer->UploadData(baseImageFileData.data(), baseImageFileData.size());
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to upload base data!" << std::endl;
        baseStorageBuffer->Destroy();
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
    ret = blendStorageBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, blendBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to allocate blend storage buffer!" << std::endl;
        baseStorageBuffer->Destroy();
        return;
    }
    ret = blendStorageBuffer->UploadData(blendImageFileData.data(), blendImageFileData.size());
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to upload blend data!" << std::endl;
        baseStorageBuffer->Destroy();
        blendStorageBuffer->Destroy();
    }
    const uint64_t blendImageBufferPrepareEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "Blend Image Buffer Prepare And Upload Time: " << blendImageBufferPrepareEnd -
            blendImageBufferPrepareStart << "ms" <<
            std::endl << std::endl;

    blendImageFileData.clear();
    blendImageFileData.resize(0);

    ret = outputStorageBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, outputBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to allocate output storage buffer!" << std::endl;
        baseStorageBuffer->Destroy();
        blendStorageBuffer->Destroy();
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
    baseImageInfo.storageBuffer = baseStorageBuffer->GetBuffer();
    BlendImageInfo blendImageInfo;
    blendImageInfo.width = blendImageWidth;
    blendImageInfo.height = blendImageHeight;
    blendImageInfo.channels = baseImageChannels;
    blendImageInfo.bufferSize = blendBufferSize;
    blendImageInfo.posX = posX;
    blendImageInfo.posY = posY;
    blendImageInfo.storageBuffer = blendStorageBuffer->GetBuffer();
    ret = blender->Apply(gpuCtx, baseImageInfo, blendImageInfo, outputStorageBuffer->GetBuffer());
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to apply blender!" << std::endl;
        return;
    }
    const uint64_t gpuProcessTimeEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "GPU Process Time: " << gpuProcessTimeEnd - gpuProcessTimeStart << "ms" << std::endl;

    ret = outputStorageBuffer->MapBuffer(outputBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to map output storage buffer!" << std::endl;
        baseStorageBuffer->Destroy();
        blendStorageBuffer->Destroy();
        outputStorageBuffer->Destroy();
        return;
    }

    ImageUtils::WritePngFile(outputFilePath, baseImageWidth, baseImageHeight, baseImageChannels,
                             outputStorageBuffer->GetMappedAddr());
    baseStorageBuffer->Destroy();
    blendStorageBuffer->Destroy();
    outputStorageBuffer->Destroy();
}
