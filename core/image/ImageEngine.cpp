//
// Created by 1234 on 2025/3/6.
//

#include "ImageEngine.h"

#ifdef OS_OPEN_HARMONY
#include <runtime/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "core/context/Context.h"
#include "core/utils/ImageUtils.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/log/Log.h"
#include "runtime/utils/TimeUtils.h"

namespace sgl::image {

VkResult ImageEngine::ProcessGpu(
    const BufferGpu &inputBuffer, const uint32_t inputWidth,
    const uint32_t inputHeight, const BufferGpu &outputBuffer,
    const uint32_t outputWidth, const uint32_t outputHeight,
    const uint32_t channels, const std::shared_ptr<IFilter> &filter) {
  const VkDeviceSize inputBufferSize = inputWidth * inputHeight * channels;
  const VkDeviceSize outputBufferSize = outputWidth * outputHeight * channels;

  std::vector<FilterImageInfo> filterInputImages;
  FilterImageInfo inputImageInfo{};
  inputImageInfo.width = inputWidth;
  inputImageInfo.height = inputHeight;
  inputImageInfo.channels = channels;
  inputImageInfo.bufferSize = inputBufferSize;
  inputImageInfo.posX = 0;
  inputImageInfo.posY = 0;
  inputImageInfo.storageBuffer = static_cast<VkBuffer>(inputBuffer.bufHandle);
  inputImageInfo.storageBufferMemory =
      static_cast<VkDeviceMemory>(inputBuffer.memHandle);
  filterInputImages.push_back(inputImageInfo);

  std::vector<FilterImageInfo> filterOutputImages;
  FilterImageInfo outputImageInfo{};
  outputImageInfo.width = outputWidth;
  outputImageInfo.height = outputHeight;
  outputImageInfo.channels = channels;
  outputImageInfo.bufferSize = outputBufferSize;
  outputImageInfo.posX = 0;
  outputImageInfo.posY = 0;
  outputImageInfo.storageBuffer = static_cast<VkBuffer>(outputBuffer.bufHandle);
  outputImageInfo.storageBufferMemory =
      static_cast<VkDeviceMemory>(outputBuffer.memHandle);
  filterInputImages.push_back(inputImageInfo);
  filterOutputImages.push_back(outputImageInfo);

  const uint64_t gpuProcessTimeStart = TimeUtils::GetCurrentMonoMs();
  const VkResult ret = filter->Apply(Context::GetInstance()->GetContext(),
                                     filterInputImages, filterOutputImages);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to apply filter!" << std::endl;
    return ret;
  }
  const uint64_t gpuProcessTimeEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "GPU Process Time: " << gpuProcessTimeEnd - gpuProcessTimeStart
           << "ms" << std::endl;

  filter->Destroy();
  Context::GetInstance()->GetContext()->Reset();

  return ret;
}

static std::shared_ptr<ImageEngine> instance = nullptr;
std::shared_ptr<ImageEngine> ImageEngine::GetInstance() {
  if (instance == nullptr) {
    instance = std::make_shared<ImageEngine>();
  }
  return instance;
}

VkResult ImageEngine::Process(const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                              const uint32_t inputWidth,
                              const uint32_t inputHeight,
                              const uint32_t outputWidth,
                              const uint32_t outputHeight,
                              const uint32_t channels, const void *uploadData,
                              const std::shared_ptr<VkGPUBuffer> &outputBuffer,
                              const std::shared_ptr<IFilter> &filter) {
  const VkDeviceSize inputBufferSize = inputWidth * inputHeight * channels;
  const VkDeviceSize outputBufferSize = outputWidth * outputHeight * channels;
  const uint64_t imageBufferPrepareStart = TimeUtils::GetCurrentMonoMs();
  VkResult ret = inputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                              inputBufferSize);

  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to allocate input GPU buffer!"
             << std::endl;
    return ret;
  }
  ret = inputBuffer->UploadData(uploadData, inputBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to upload data!" << std::endl;
    return ret;
  }
  const uint64_t imageBufferPrepareEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "Image Buffer Prepare And Upload Time: "
           << imageBufferPrepareEnd - imageBufferPrepareStart << "ms"
           << std::endl;

  ret = outputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                      outputBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to allocate output GPU buffer!"
             << std::endl;
    return ret;
  }

  std::vector<FilterImageInfo> filterInputImages;
  FilterImageInfo inputImageInfo{};
  inputImageInfo.width = inputWidth;
  inputImageInfo.height = inputHeight;
  inputImageInfo.channels = channels;
  inputImageInfo.bufferSize = inputBufferSize;
  inputImageInfo.posX = 0;
  inputImageInfo.posY = 0;
  inputImageInfo.storageBuffer = inputBuffer->GetBuffer();
  inputImageInfo.storageBufferMemory = inputBuffer->GetDeviceMemory();
  filterInputImages.push_back(inputImageInfo);

  std::vector<FilterImageInfo> filterOutputImages;
  FilterImageInfo outputImageInfo{};
  outputImageInfo.width = outputWidth;
  outputImageInfo.height = outputHeight;
  outputImageInfo.channels = channels;
  outputImageInfo.bufferSize = outputBufferSize;
  outputImageInfo.posX = 0;
  outputImageInfo.posY = 0;
  outputImageInfo.storageBuffer = outputBuffer->GetBuffer();
  outputImageInfo.storageBufferMemory = outputBuffer->GetDeviceMemory();
  filterOutputImages.push_back(outputImageInfo);

  const uint64_t gpuProcessTimeStart = TimeUtils::GetCurrentMonoMs();
  ret = filter->Apply(Context::GetInstance()->GetContext(), filterInputImages,
                      filterOutputImages);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to apply filter!" << std::endl;
    return ret;
  }
  const uint64_t gpuProcessTimeEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "GPU Process Time: " << gpuProcessTimeEnd - gpuProcessTimeStart
           << "ms" << std::endl;

  filter->Destroy();
  Context::GetInstance()->GetContext()->Reset();

  return ret;
}

void ImageEngine::ProcessFromCpuAddr(const ImageInfoCpu &input,
                                     const ImageInfoCpu &output,
                                     const std::shared_ptr<IFilter> &filter) {
  if (input.channels != output.channels) {
    Logger() << "Input and output channel must be same size!" << std::endl;
    return;
  }
  if (input.width != output.width || input.height != output.height) {
    Logger() << "Scale from (" << input.width << "," << input.height << ") to ("
             << output.width << "," << output.height << ")" << std::endl;
  }

  Logger() << "[IMAGE SIZE]" << "WIDTH " << input.width << ", HEIGHT "
           << input.height << std::endl;
  const VkDeviceSize outputBufferSize =
      output.width * output.height * output.channels;
  const auto inputStorageBuffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
  const auto outputStorageBuffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());

  const VkResult ret = Process(inputStorageBuffer, input.width, input.height,
                               output.width, output.height, input.channels,
                               input.data, outputStorageBuffer, filter);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to process input storage buffer, err="
             << string_VkResult(ret) << std::endl;
    return;
  }

  const uint64_t imageDownloadStart = TimeUtils::GetCurrentMonoMs();
  outputStorageBuffer->DownloadData(output.data, outputBufferSize);
  const uint64_t imageDownloadEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "Image Download Time: " << imageDownloadEnd - imageDownloadStart
           << "ms" << std::endl;

  inputStorageBuffer->Destroy();
  outputStorageBuffer->Destroy();
}

void ImageEngine::Process(const ImageInfo &input, const ImageInfo &output,
                          const std::shared_ptr<IFilter> &filter) const {
  if (input.type == SGL_IMAGE_TYPE_CPU && output.type == SGL_IMAGE_TYPE_CPU) {
    return ProcessFromCpuAddr(input.info.cpu, output.info.cpu, filter);
  }
  if (input.type == SGL_IMAGE_TYPE_GPU && output.type == SGL_IMAGE_TYPE_GPU) {
    return ProcessFromGpuAddr(input.info.gpu, output.info.gpu, filter);
  }
  Logger() << Logger::ERROR << "type of in and out should all be GPU/CPU"
           << std::endl;
}

void ImageEngine::ProcessFromCpuAddr(const std::vector<ImageInfoCpu> &inputs,
                                     const std::vector<ImageInfoCpu> &outputs,
                                     const std::shared_ptr<IFilter> &filter) {
  VkResult ret = VK_SUCCESS;
  std::vector<std::shared_ptr<VkGPUBuffer>> inputBuffers;
  std::vector<FilterImageInfo> filterInputImages;
  const uint64_t inputBufferPrepareStart = TimeUtils::GetCurrentMonoMs();
  for (const ImageInfoCpu &input : inputs) {
    const auto inputBuffer =
        std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
    const VkDeviceSize inputBufferSize =
        input.width * input.height * input.channels;
    ret = inputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                       inputBufferSize);
    if (ret != VK_SUCCESS) {
      Logger() << "Failed to allocate input storage buffer!" << std::endl;
      for (const std::shared_ptr<VkGPUBuffer> &buffer : inputBuffers) {
        buffer->Destroy();
      }
      return;
    }
    inputBuffers.push_back(inputBuffer);
    ret = inputBuffer->UploadData(input.data, inputBufferSize);
    if (ret != VK_SUCCESS) {
      Logger() << "Failed to upload base data!" << std::endl;
      for (const std::shared_ptr<VkGPUBuffer> &buffer : inputBuffers) {
        buffer->Destroy();
      }
      return;
    }
    FilterImageInfo inputImageInfo{};
    inputImageInfo.width = input.width;
    inputImageInfo.height = input.height;
    inputImageInfo.channels = input.channels;
    inputImageInfo.bufferSize = inputBufferSize;
    inputImageInfo.posX = 0;
    inputImageInfo.posY = 0;
    inputImageInfo.storageBuffer = inputBuffer->GetBuffer();
    inputImageInfo.storageBufferMemory = inputBuffer->GetDeviceMemory();
    filterInputImages.push_back(inputImageInfo);
  }
  const uint64_t inputBufferPrepareEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "Input Buffer Prepare And Upload Time: "
           << inputBufferPrepareEnd - inputBufferPrepareStart << "ms"
           << std::endl;

  std::vector<std::shared_ptr<VkGPUBuffer>> outputBuffers;
  std::vector<FilterImageInfo> filterOutputImages;
  const uint64_t outputBufferPrepareStart = TimeUtils::GetCurrentMonoMs();
  for (const ImageInfoCpu &output : outputs) {
    const auto outputBuffer =
        std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
    const VkDeviceSize outputBufferSize =
        output.width * output.height * output.channels;
    ret = outputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                        outputBufferSize);
    if (ret != VK_SUCCESS) {
      for (const std::shared_ptr<VkGPUBuffer> &buffer : inputBuffers) {
        buffer->Destroy();
      }
      for (const std::shared_ptr<VkGPUBuffer> &buffer : outputBuffers) {
        buffer->Destroy();
      }
      Logger() << "Failed to allocate output storage buffer!" << std::endl;
      return;
    }
    outputBuffers.push_back(outputBuffer);

    FilterImageInfo outputImageInfo{};
    outputImageInfo.width = output.width;
    outputImageInfo.height = output.height;
    outputImageInfo.channels = output.channels;
    outputImageInfo.bufferSize = outputBufferSize;
    outputImageInfo.posX = 0;
    outputImageInfo.posY = 0;
    outputImageInfo.storageBuffer = outputBuffer->GetBuffer();
    outputImageInfo.storageBufferMemory = outputBuffer->GetDeviceMemory();
    filterOutputImages.push_back(outputImageInfo);
  }
  const uint64_t outputBufferPrepareEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "Output Buffer Prepare And Upload Time: "
           << outputBufferPrepareEnd - outputBufferPrepareStart << "ms"
           << std::endl;

  const uint64_t gpuProcessTimeStart = TimeUtils::GetCurrentMonoMs();
  ret = filter->Apply(Context::GetInstance()->GetContext(), filterInputImages,
                      filterOutputImages);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to apply blender!" << std::endl;
    for (const std::shared_ptr<VkGPUBuffer> &buffer : inputBuffers) {
      buffer->Destroy();
    }
    for (const std::shared_ptr<VkGPUBuffer> &buffer : outputBuffers) {
      buffer->Destroy();
    }
    filter->Destroy();
    Context::GetInstance()->GetContext()->Reset();
    return;
  }
  const uint64_t gpuProcessTimeEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "GPU Process Time: " << gpuProcessTimeEnd - gpuProcessTimeStart
           << "ms" << std::endl;

  const uint64_t imageDownloadStart = TimeUtils::GetCurrentMonoMs();
  for (size_t i = 0; i < filterOutputImages.size(); i++) {
    const auto &buffer = outputBuffers[i];
    if (buffer == nullptr) {
      break;
    }
    ret =
        buffer->DownloadData(outputs[i].data, filterOutputImages[i].bufferSize);
    if (ret != VK_SUCCESS) {
      Logger() << "Failed to download data from output storage buffer!"
               << std::endl;
      break;
    }
  }
  const uint64_t imageDownloadEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "Image Download Time: " << imageDownloadEnd - imageDownloadStart
           << "ms" << std::endl;

  for (const std::shared_ptr<VkGPUBuffer> &buffer : inputBuffers) {
    buffer->Destroy();
  }
  for (const std::shared_ptr<VkGPUBuffer> &buffer : outputBuffers) {
    buffer->Destroy();
  }
  filter->Destroy();
  Context::GetInstance()->GetContext()->Reset();

  inputBuffers.clear();
  inputBuffers.resize(0);
  outputBuffers.clear();
  outputBuffers.resize(0);
}

void ImageEngine::ProcessFromGpuAddr(const ImageInfoGpu &input,
                                     const ImageInfoGpu &output,
                                     const std::shared_ptr<IFilter> &filter) {
  if (input.channels != output.channels) {
    Logger() << "Input and output channel must be same size!" << std::endl;
    return;
  }
  if (input.width != output.width || input.height != output.height) {
    Logger() << "Scale from (" << input.width << "," << input.height << ") to ("
             << output.width << "," << output.height << ")" << std::endl;
  }

  Logger() << "[IMAGE SIZE]" << "WIDTH " << input.width << ", HEIGHT "
           << input.height << std::endl;
  const VkResult ret =
      ProcessGpu(input.gpuBuf, input.width, input.height, output.gpuBuf,
                 output.width, output.height, input.channels, filter);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to process input storage buffer, err="
             << string_VkResult(ret) << std::endl;
  }
}

void ImageEngine::Process(const std::vector<ImageInfo> &inputs,
                          const std::vector<ImageInfo> &outputs,
                          const std::shared_ptr<IFilter> &filter) {
  if (inputs[0].type == SGL_IMAGE_TYPE_CPU &&
      outputs[0].type == SGL_IMAGE_TYPE_CPU) {

    std::vector<ImageInfoCpu> cpuInputs;
    std::vector<ImageInfoCpu> cpuOutputs;

    cpuInputs.reserve(inputs.size());
    for (const auto &[type, info] : inputs) {
      cpuInputs.push_back(info.cpu);
    }
    cpuOutputs.reserve(outputs.size());
    for (const auto &[type, info] : outputs) {
      cpuOutputs.push_back(info.cpu);
    }

    return ProcessFromCpuAddr(cpuInputs, cpuOutputs, filter);
  }
  if (inputs[0].type == SGL_IMAGE_TYPE_GPU &&
      outputs[0].type == SGL_IMAGE_TYPE_GPU) {
    // TODO: Process with GPU addr
  } else {
    Logger() << Logger::ERROR << "type of in and out should all be GPU/CPU"
             << std::endl;
  }
}

void ImageEngine::Process(const char *inputFilePath, const char *outputFilePath,
                          const std::shared_ptr<IFilter> &filter) {
  uint32_t imageWidth = 0, imageHeight = 0, channels = 0;
  std::vector<char> inputFileData = ImageUtils::ReadPngFile(
      inputFilePath, &imageWidth, &imageHeight, &channels);

  if (inputFileData.empty()) {
    Logger() << "Failed to read input file!" << std::endl;
    return;
  }
  Logger() << "Image width= " << imageWidth << ", height=" << imageHeight
           << ", channels=" << channels << std::endl;
  const VkDeviceSize outputBufferSize = imageWidth * imageHeight * channels;

  const auto inputStorageBuffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
  const auto outputStorageBuffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());

  VkResult ret = Process(inputStorageBuffer, imageWidth, imageHeight,
                         imageWidth, imageHeight, channels,
                         inputFileData.data(), outputStorageBuffer, filter);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to process input storage buffer, err="
             << string_VkResult(ret) << std::endl;
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

  ImageUtils::WritePngFile(outputFilePath, imageWidth, imageHeight, channels,
                           outputStorageBuffer->GetMappedAddr());
  inputStorageBuffer->Destroy();
  outputStorageBuffer->Destroy();
}

void ImageEngine::Process(const char *inputFilePath, const char *outputFilePath,
                          const uint32_t newWidth, const uint32_t newHeight,
                          const std::shared_ptr<IFilter> &filter) {
  uint32_t imageWidth = 0, imageHeight = 0, channels = 0;
  std::vector<char> inputFileData = ImageUtils::ReadPngFile(
      inputFilePath, &imageWidth, &imageHeight, &channels);
  if (inputFileData.empty()) {
    Logger() << "Failed to read input file!" << std::endl;
    return;
  }
  Logger() << "Image width= " << imageWidth << ", height=" << imageHeight
           << ", channels=" << channels << std::endl;
  const VkDeviceSize outputBufferSize = newWidth * newHeight * channels;

  const auto inputStorageBuffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
  const auto outputStorageBuffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());

  VkResult ret =
      Process(inputStorageBuffer, imageWidth, imageHeight, newWidth, newHeight,
              channels, inputFileData.data(), outputStorageBuffer, filter);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to process input storage buffer, err="
             << string_VkResult(ret) << std::endl;
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

  const void *outputDataAddr = outputStorageBuffer->GetMappedAddr();
  ImageUtils::WritePngFile(outputFilePath, newWidth, newHeight, channels,
                           outputDataAddr);
  inputStorageBuffer->Destroy();
  outputStorageBuffer->Destroy();
}

void ImageEngine::Process(const char *baseFilePath, const char *blendFilePath,
                          const uint32_t posX, const uint32_t posY,
                          const char *outputFilePath,
                          const std::shared_ptr<IBlender> &blender) {
  uint32_t baseImageWidth = 0, baseImageHeight = 0, baseImageChannels = 0;
  std::vector<char> baseImageFileData = ImageUtils::ReadPngFile(
      baseFilePath, &baseImageWidth, &baseImageHeight, &baseImageChannels);
  if (baseImageFileData.empty()) {
    Logger() << "Failed to read input base file!" << std::endl;
    return;
  }
  Logger() << "Base Image width= " << baseImageWidth
           << ", height=" << baseImageHeight
           << ", channels=" << baseImageChannels << std::endl;

  uint32_t blendImageWidth = 0, blendImageHeight = 0, blendImageChannels = 0;
  std::vector<char> blendImageFileData = ImageUtils::ReadPngFile(
      blendFilePath, &blendImageWidth, &blendImageHeight, &blendImageChannels);
  if (blendImageFileData.empty()) {
    Logger() << "Failed to read input blend file!" << std::endl;
    return;
  }
  Logger() << "Blend Image width= " << blendImageWidth
           << ", height=" << blendImageHeight
           << ", channels=" << blendImageChannels << std::endl;

  const VkDeviceSize outputBufferSize =
      baseImageWidth * baseImageHeight * baseImageChannels;
  const auto baseStorageBuffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
  const auto blendStorageBuffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
  const auto outputStorageBuffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());

  const uint64_t baseImageBufferPrepareStart = TimeUtils::GetCurrentMonoMs();
  const VkDeviceSize baseBufferSize =
      baseImageWidth * baseImageHeight * baseImageChannels;
  VkResult ret = baseStorageBuffer->AllocateAndBind(
      GPU_BUFFER_TYPE_STORAGE_SHARED, baseBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to allocate base storage buffer!" << std::endl;
    return;
  }
  ret = baseStorageBuffer->UploadData(baseImageFileData.data(),
                                      baseImageFileData.size());
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to upload base data!" << std::endl;
    baseStorageBuffer->Destroy();
    return;
  }
  const uint64_t baseImageBufferPrepareEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "Base Image Buffer Prepare And Upload Time: "
           << baseImageBufferPrepareEnd - baseImageBufferPrepareStart << "ms"
           << std::endl;
  baseImageFileData.clear();
  baseImageFileData.resize(0);

  const VkDeviceSize blendBufferSize =
      blendImageWidth * blendImageHeight * blendImageChannels;
  const uint64_t blendImageBufferPrepareStart = TimeUtils::GetCurrentMonoMs();
  ret = blendStorageBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                            blendBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to allocate blend storage buffer!" << std::endl;
    baseStorageBuffer->Destroy();
    return;
  }
  ret = blendStorageBuffer->UploadData(blendImageFileData.data(),
                                       blendImageFileData.size());
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to upload blend data!" << std::endl;
    baseStorageBuffer->Destroy();
    blendStorageBuffer->Destroy();
  }
  const uint64_t blendImageBufferPrepareEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "Blend Image Buffer Prepare And Upload Time: "
           << blendImageBufferPrepareEnd - blendImageBufferPrepareStart << "ms"
           << std::endl
           << std::endl;

  blendImageFileData.clear();
  blendImageFileData.resize(0);

  ret = outputStorageBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                             outputBufferSize);
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
  ret = blender->Apply(Context::GetInstance()->GetContext(), baseImageInfo,
                       blendImageInfo, outputStorageBuffer->GetBuffer());
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to apply blender!" << std::endl;
    return;
  }
  const uint64_t gpuProcessTimeEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "GPU Process Time: " << gpuProcessTimeEnd - gpuProcessTimeStart
           << "ms" << std::endl;

  ret = outputStorageBuffer->MapBuffer(outputBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to map output storage buffer!" << std::endl;
    baseStorageBuffer->Destroy();
    blendStorageBuffer->Destroy();
    outputStorageBuffer->Destroy();
    return;
  }

  ImageUtils::WritePngFile(outputFilePath, baseImageWidth, baseImageHeight,
                           baseImageChannels,
                           outputStorageBuffer->GetMappedAddr());
  baseStorageBuffer->Destroy();
  blendStorageBuffer->Destroy();
  outputStorageBuffer->Destroy();
}
} // namespace sgl::image
