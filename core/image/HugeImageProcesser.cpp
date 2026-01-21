//
// Created by neo on 2026/1/20.
//

#include "HugeImageProcesser.h"

#include "core/context/Context.h"
#include "runtime/log/Log.h"
#include "runtime/utils/TimeUtils.h"
#include "tile/TileBasedFilter.h"

#include <vulkan/vk_enum_string_helper.h>

HugeImageProcesser::HugeImageProcesser(
    const std::shared_ptr<HugeImageInfo> &input) {
  this->infoInfo = input;
  this->outputBuffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
  this->tilePlaceholderBuffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
  this->inputStorageBuffers.clear();
}

VkResult HugeImageProcesser::Init() const {
  VkResult ret = VK_SUCCESS;

  if (this->infoInfo->height < TILE_HEIGHT * 20) {
    Logger() << "Small that 20 Tiles, suggest use normal process!" << std::endl;
    ret = VK_ERROR_UNKNOWN;
    return ret;
  }

  const VkDeviceSize outputBufferSize =
      this->infoInfo->width * TILE_HEIGHT * this->infoInfo->channels;
  ret = this->outputBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                            outputBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to allocate output buffer!" << std::endl;
    return ret;
  }

  ret = this->tilePlaceholderBuffer->AllocateAndBind(
      GPU_BUFFER_TYPE_STORAGE_SHARED, outputBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to allocate placeholder buffer!" << std::endl;
    return ret;
  }
  return ret;
}

VkResult HugeImageProcesser::CreateTileBufferCache(const int tileIdx) {
  VkResult ret = VK_SUCCESS;
  const VkDeviceSize tileBufferSize =
      this->infoInfo->width * TILE_HEIGHT * this->infoInfo->channels;
  const auto buffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
  ret = buffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, tileBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to allocate tile buffer!" << std::endl;
    return ret;
  }
  ret = buffer->MapBuffer(tileBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to map tile buffer!" << std::endl;
    return ret;
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

  this->buffer_cache.emplace(tileIdx, buffer);

  return ret;
}

VkResult HugeImageProcesser::CheckAndCreateTilesBuffer(
    const std::vector<int> &tilesIdx) {
  VkResult ret = VK_SUCCESS;

  const uint32_t maxTileIdx = this->infoInfo->height / TILE_HEIGHT;
  for (const int tileIdx : tilesIdx) {
    if ((tileIdx < 0) || (tileIdx > maxTileIdx)) {
      continue;
    }
    if (!this->buffer_cache.contains(tileIdx)) {
      ret = CreateTileBufferCache(tileIdx);
      if (ret != VK_SUCCESS) {
        Logger() << "Failed to create tile buffer cache!" << std::endl;
        return ret;
      }
    }
  }
  return ret;
}

VkResult HugeImageProcesser::CreateTileBuffersCache(const int tileIdx) {
  VkResult ret = VK_SUCCESS;

  for (auto iter = this->buffer_cache.begin();
       iter != this->buffer_cache.end();) {
    if ((iter->first < (tileIdx - 4)) || (iter->first > (tileIdx + 4))) {
      iter = this->buffer_cache.erase(iter);
    } else {
      ++iter;
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

VkResult HugeImageProcesser::CheckAndPrepareInputBuffers(
    const std::vector<int> &tilesIdx) {
  this->inputStorageBuffers.clear();
  const uint32_t maxTileIdx = this->infoInfo->height / TILE_HEIGHT;
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

VkResult HugeImageProcesser::PrepareInputBufferForTile(const int tileIdx) {
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

VkResult HugeImageProcesser::Process(
    const int tileIdx, const std::shared_ptr<TileBasedFilter> &filter) const {
  VkResult ret = VK_SUCCESS;
  std::vector<FilterImageInfo> filterInputImages;
  for (const auto &buffer : this->inputStorageBuffers) {
    FilterImageInfo inputImageInfo{};
    inputImageInfo.width = this->infoInfo->width;
    inputImageInfo.height = TILE_HEIGHT;
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

void HugeImageProcesser::Process(const int tileIdx,
                                 const std::shared_ptr<TileBasedFilter> &filter,
                                 const std::shared_ptr<ImageInfoCpu> &output) {
  if (this->infoInfo->channels != output->channels) {
    Logger() << "Input and output channel must be same size!" << std::endl;
    return;
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

  VkResult ret = this->PrepareInputBufferForTile(tileIdx);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to prepare input buffer!" << std::endl;
    return;
  }

  ret = this->Process(tileIdx, filter);
  if (ret != VK_SUCCESS) {
    Logger() << "Failed to process input storage buffer, err="
             << string_VkResult(ret) << std::endl;
    return;
  }
  this->inputStorageBuffers.clear();

  const uint64_t imageDownloadStart = TimeUtils::GetCurrentMonoMs();
  this->outputBuffer->DownloadData(
      output->data, output->width * output->height * output->channels);
  const uint64_t imageDownloadEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "Image Download Time: " << imageDownloadEnd - imageDownloadStart
           << "ms" << std::endl;
}

std::shared_ptr<VkGPUBuffer>
HugeImageProcesser::GetOutputBuffer(int tileIdx) const {
  return this->outputBuffer;
}