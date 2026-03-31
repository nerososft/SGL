//
// Created by neo on 2026/1/20.
//

#include "HugeImageProcessor.h"

#include "core/context/Context.h"
#include "runtime/log/Log.h"
#include "runtime/utils/TimeUtils.h"
#include "tile_filters/ITileBasedFilter.h"

#include <future>
#ifdef OS_OPEN_HARMONY
#include <runtime/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

HugeImageProcessor::HugeImageProcessor(
    const std::shared_ptr<HugeImageInfo> &input) {
  this->infoInfo = input;
  this->outputBuffers.fill(nullptr);
  this->tilePlaceholderBuffer = nullptr;
  this->inputStorageBuffers.clear();
}

VkResult HugeImageProcessor::Init() const {
  VkResult ret = VK_SUCCESS;

  if (this->infoInfo->height < TILE_HEIGHT * 10) {
    Logger() << "Small that 10 Tiles, suggest use normal process!" << std::endl;
    ret = VK_ERROR_UNKNOWN;
    return ret;
  }

  return ret;
}
void HugeImageProcessor::Destroy() {
  this->WaitForPrefetch();
  for (auto &outputBuffer : this->outputBuffers) {
    if (outputBuffer != nullptr) {
      outputBuffer->Destroy();
      outputBuffer = nullptr;
    }
  }
  if (this->tilePlaceholderBuffer != nullptr) {
    this->tilePlaceholderBuffer->Destroy();
    this->tilePlaceholderBuffer = nullptr;
  }

  for (const auto &buffer : this->freeBufferList) {
    buffer->Destroy();
  }

  for (const auto &buffer : this->buffer_cache) {
    buffer.second->Destroy();
  }
}

VkResult HugeImageProcessor::PrepareOutputBuffer(
    const std::shared_ptr<ImageInfoCpu> &output) {
  VkResult ret = VK_SUCCESS;
  const VkDeviceSize outputBufferSize =
      output->width * TILE_HEIGHT * output->channels;
  for (auto &outputBuffer : this->outputBuffers) {
    if (outputBuffer != nullptr) {
      continue;
    }
    outputBuffer =
        std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
    ret = outputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                        outputBufferSize);
    if (ret != VK_SUCCESS) {
      Logger() << "Failed to allocate output buffer!" << std::endl;
      return ret;
    }
  }

  if (this->tilePlaceholderBuffer != nullptr) {
    return ret;
  }
  this->tilePlaceholderBuffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
  ret = this->tilePlaceholderBuffer->AllocateAndBind(
      GPU_BUFFER_TYPE_STORAGE_SHARED, outputBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to allocate placeholder buffer!" << std::endl;
    return ret;
  }
  return ret;
}

std::shared_ptr<VkGPUBuffer> HugeImageProcessor::GetBufferFromFreeList() {
  std::shared_ptr<VkGPUBuffer> buf = nullptr;
  {
    std::lock_guard<std::mutex> lock(this->bufferCacheMutex);
    if (!freeBufferList.empty()) {
      buf = this->freeBufferList.back();
      this->freeBufferList.pop_back();
      Logger() << "Reuse free tile buffer!" << std::endl;
      return buf;
    }
  }
  if (buf == nullptr) {
    const VkDeviceSize tileBufferSize =
        this->infoInfo->width * TILE_HEIGHT * this->infoInfo->channels;
    buf = std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
    VkResult ret =
        buf->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, tileBufferSize);
    if (ret != VK_SUCCESS) {
      Logger() << "Failed to allocate tile buffer!" << std::endl;
      return nullptr;
    }
    ret = buf->MapBuffer(tileBufferSize);
    if (ret != VK_SUCCESS) {
      Logger() << "Failed to map tile buffer!" << std::endl;
      buf->Destroy();
      return nullptr;
    }
    Logger() << "Successfully alloc tile buffer!" << std::endl;
  }
  return buf;
}

void HugeImageProcessor::ReturnBufferToFreeList(
    const std::shared_ptr<VkGPUBuffer> &buffer) {
  std::lock_guard<std::mutex> lock(this->bufferCacheMutex);
  this->freeBufferList.push_back(buffer);
}

int HugeImageProcessor::GetMaxTileIdx() const {
  return static_cast<int>(this->infoInfo->height / TILE_HEIGHT);
}

int HugeImageProcessor::GetOutputSlot(const int tileIdx) const {
  return tileIdx % static_cast<int>(this->outputBuffers.size());
}

void HugeImageProcessor::WaitForPrefetch() {
  if (!this->prefetchFuture.valid()) {
    this->prefetchedTileIdx = -1;
    return;
  }

  const VkResult ret = this->prefetchFuture.get();
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to prefetch tile " << this->prefetchedTileIdx
             << ", err=" << string_VkResult(ret) << std::endl;
  }
  this->prefetchedTileIdx = -1;
}

void HugeImageProcessor::StartPrefetchForNextTile(const int currentTileIdx) {
  const int nextTileIdx = currentTileIdx + 5;
  if (nextTileIdx > this->GetMaxTileIdx()) {
    return;
  }

  {
    std::lock_guard<std::mutex> lock(this->bufferCacheMutex);
    if (this->buffer_cache.contains(nextTileIdx)) {
      return;
    }
  }

  this->prefetchedTileIdx = nextTileIdx;
  this->prefetchFuture = std::async(std::launch::async, [this, nextTileIdx] {
    Logger() << "Prefetch tile buffer for tile " << nextTileIdx << std::endl;
    return this->CreateTileBufferCache(nextTileIdx);
  });
}

VkResult HugeImageProcessor::CreateTileBufferCache(const int tileIdx) {
  {
    std::lock_guard<std::mutex> lock(this->bufferCacheMutex);
    if (this->buffer_cache.contains(tileIdx)) {
      return VK_SUCCESS;
    }
  }

  const auto buffer = this->GetBufferFromFreeList();
  if (buffer == nullptr) {
    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
  }

  void *bufferAddr = buffer->GetMappedAddr();
  const size_t beginRow = tileIdx * TILE_HEIGHT;
  const size_t rowSize = this->infoInfo->width * this->infoInfo->channels;
  for (size_t row = 0; row < TILE_HEIGHT; row++) {
    void *rowData = this->infoInfo->getRowData(beginRow + row);
    const auto targetAddr = reinterpret_cast<void *>(
        reinterpret_cast<size_t>(bufferAddr) + row * rowSize);
    memcpy(targetAddr, rowData, rowSize);
    this->infoInfo->afterRowDataUse(rowData);
  }

  {
    std::lock_guard<std::mutex> lock(this->bufferCacheMutex);
    if (this->buffer_cache.contains(tileIdx)) {
      this->freeBufferList.push_back(buffer);
      return VK_SUCCESS;
    }
    this->buffer_cache.emplace(tileIdx, buffer);
  }

  return VK_SUCCESS;
}

VkResult HugeImageProcessor::CheckAndCreateTilesBuffer(
    const std::vector<int> &tilesIdx) {
  VkResult ret = VK_SUCCESS;

  const int maxTileIdx = this->GetMaxTileIdx();
  for (const int tileIdx : tilesIdx) {
    if ((tileIdx < 0) || (tileIdx > maxTileIdx)) {
      continue;
    }
    {
      std::lock_guard<std::mutex> lock(this->bufferCacheMutex);
      if (this->buffer_cache.contains(tileIdx)) {
        continue;
      }
    }
    ret = CreateTileBufferCache(tileIdx);
    if (ret != VK_SUCCESS) {
      Logger() << "Failed to create tile buffer cache!" << std::endl;
      return ret;
    }
  }
  return ret;
}

VkResult HugeImageProcessor::CreateTileBuffersCache(const int tileIdx) {
  VkResult ret = VK_SUCCESS;

  {
    std::lock_guard<std::mutex> lock(this->bufferCacheMutex);
    for (auto iter = this->buffer_cache.begin();
         iter != this->buffer_cache.end();) {
      if ((iter->first < (tileIdx - 4)) || (iter->first > (tileIdx + 4))) {
        this->freeBufferList.push_back(iter->second);
        iter = this->buffer_cache.erase(iter);
      } else {
        ++iter;
      }
    }
  }

  ret = CheckAndCreateTilesBuffer({tileIdx - 4, tileIdx - 3, tileIdx - 2,
                                   tileIdx - 1, tileIdx, tileIdx + 1,
                                   tileIdx + 2, tileIdx + 3, tileIdx + 4});
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create input buffer cache for tile " << 0 << "!"
             << std::endl;
    return ret;
  }

  return ret;
}

VkResult HugeImageProcessor::CheckAndPrepareInputBuffers(
    const std::vector<int> &tilesIdx) {
  this->inputStorageBuffers.clear();
  const int maxTileIdx = this->GetMaxTileIdx();
  std::lock_guard<std::mutex> lock(this->bufferCacheMutex);
  for (const int tileIdx : tilesIdx) {
    if ((tileIdx < 0) || (tileIdx > maxTileIdx)) {
      this->inputStorageBuffers.push_back(this->tilePlaceholderBuffer);
    } else {
      if (!this->buffer_cache.contains(tileIdx)) {
        Logger() << "Input buffer cache does not exist!" << std::endl;
        return VK_ERROR_UNKNOWN;
      }
      this->inputStorageBuffers.push_back(this->buffer_cache[tileIdx]);
    }
  }
  return VK_SUCCESS;
}

VkResult HugeImageProcessor::PrepareInputBufferForTile(const int tileIdx) {
  VkResult ret = VK_SUCCESS;

  ret = this->CreateTileBuffersCache(tileIdx);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to create input buffer cache!" << std::endl;
    return ret;
  }

  ret = CheckAndPrepareInputBuffers({tileIdx - 4, tileIdx - 3, tileIdx - 2,
                                     tileIdx - 1, tileIdx, tileIdx + 1,
                                     tileIdx + 2, tileIdx + 3, tileIdx + 4});
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to prepare input buffers!" << std::endl;
  }

  return ret;
}

VkResult HugeImageProcessor::Process(
    const int tileIdx, const std::shared_ptr<ITileBasedFilter> &filter) const {
  VkResult ret = VK_SUCCESS;
  const auto outputBuffer = this->GetOutputBuffer(tileIdx);
  if (outputBuffer == nullptr) {
    Logger() << "Output buffer is not ready!" << std::endl;
    return VK_ERROR_INITIALIZATION_FAILED;
  }
  std::vector<FilterImageInfo> filterInputImages;
  for (const auto &buffer : this->inputStorageBuffers) {
    FilterImageInfo inputImageInfo{};
    inputImageInfo.width = this->infoInfo->width;
    inputImageInfo.height = this->infoInfo->height;
    inputImageInfo.channels = this->infoInfo->channels;
    inputImageInfo.bufferSize =
        this->infoInfo->width * TILE_HEIGHT * this->infoInfo->channels;
    inputImageInfo.posX = 0;
    inputImageInfo.posY = 0;
    inputImageInfo.storageBuffer = buffer->GetBuffer();
    inputImageInfo.storageBufferMemory = buffer->GetDeviceMemory();
    filterInputImages.push_back(inputImageInfo);
  }

  std::vector<FilterImageInfo> filterOutputImages;
  FilterImageInfo outputImageInfo{};
  outputImageInfo.width = this->infoInfo->width;
  outputImageInfo.height = TILE_HEIGHT;
  outputImageInfo.channels = this->infoInfo->channels;
  outputImageInfo.bufferSize =
      this->infoInfo->width * TILE_HEIGHT * this->infoInfo->channels;
  outputImageInfo.posX = 0;
  outputImageInfo.posY = 0;
  outputImageInfo.storageBuffer = outputBuffer->GetBuffer();
  outputImageInfo.storageBufferMemory = outputBuffer->GetDeviceMemory();
  filterOutputImages.push_back(outputImageInfo);

  const uint64_t gpuProcessTimeStart = TimeUtils::GetCurrentMonoMs();
  ret = filter->Apply(Context::GetInstance()->GetContext(), tileIdx,
                      filterInputImages, filterOutputImages);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to apply filter!" << std::endl;
    return ret;
  }
  const uint64_t gpuProcessTimeEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "GPU Process Time: " << gpuProcessTimeEnd - gpuProcessTimeStart
           << "ms" << std::endl;
  Context::GetInstance()->GetContext()->Reset();
  return ret;
}

void *
HugeImageProcessor::Process(const int tileIdx,
                            const std::shared_ptr<ITileBasedFilter> &filter,
                            const std::shared_ptr<ImageInfoCpu> &output) {
  this->WaitForPrefetch();
  if (this->infoInfo->channels != output->channels) {
    Logger() << "Input and output channel must be same size!" << std::endl;
    return nullptr;
  }
  if (output->width != this->infoInfo->width) {
    Logger() << "Input width must be same size!" << std::endl;
  }
  if (output->height != 256) {
    Logger() << "Input height must be 256 for tile process!" << std::endl;
  }

  Logger() << "[IMAGE SIZE]" << "WIDTH " << this->infoInfo->width << ", HEIGHT "
           << this->infoInfo->height << std::endl;
  Logger() << "[OUTPUT SIZE]" << "TILE " << tileIdx << ", WIDTH "
           << output->width << ", HEIGHT " << output->height << std::endl;

  VkResult ret = this->PrepareOutputBuffer(output);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to prepare output buffer!" << std::endl;
    return nullptr;
  }
  const auto outputBuffer = this->GetOutputBuffer(tileIdx);
  if (outputBuffer == nullptr) {
    Logger() << "Failed to select output buffer for tile " << tileIdx
             << std::endl;
    return nullptr;
  }

  ret = this->PrepareInputBufferForTile(tileIdx);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to prepare input buffer!" << std::endl;
    return nullptr;
  }

  // Overlap CPU row reads for the next window's new tile with current GPU work.
  this->StartPrefetchForNextTile(tileIdx);

  ret = this->Process(tileIdx, filter);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to process input storage buffer, err="
             << string_VkResult(ret) << std::endl;
    return nullptr;
  }
  this->inputStorageBuffers.clear();

  if (output->data != nullptr) {
    const uint64_t imageDownloadStart = TimeUtils::GetCurrentMonoMs();
    outputBuffer->DownloadData(
        output->data, output->width * output->height * output->channels);
    const uint64_t imageDownloadEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "Image Download Time: " << imageDownloadEnd - imageDownloadStart
             << "ms" << std::endl;
  }

  outputBuffer->MapBuffer(output->width * output->height * output->channels);

  return outputBuffer->GetMappedAddr();
}

std::shared_ptr<VkGPUBuffer>
HugeImageProcessor::GetOutputBuffer(const int tileIdx) const {
  return this->outputBuffers[this->GetOutputSlot(tileIdx)];
}
