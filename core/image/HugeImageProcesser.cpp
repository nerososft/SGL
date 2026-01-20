//
// Created by neo on 2026/1/20.
//

#include "HugeImageProcesser.h"

#include "core/context/Context.h"
#include "runtime/log/Log.h"
#include "runtime/utils/TimeUtils.h"

#include <vulkan/vk_enum_string_helper.h>

HugeImageProcesser::HugeImageProcesser(
    const std::shared_ptr<HugeImageInfo> &input) {
  this->infoInfo = input;
  this->outputBuffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
  this->inputStorageBuffers.resize(PROCESS_TILES_BUFFER_CNT);
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
  return ret;
}
VkResult HugeImageProcesser::CheckAndCreateTilesBuffer(
    const std::vector<int> &tilesIdx) const {
  VkResult ret = VK_SUCCESS;
  const uint32_t maxTileIdx = this->infoInfo->height / TILE_HEIGHT;
  for (const int tileIdx : tilesIdx) {
    if (tileIdx < 0) {
      continue;
    }
    if (tileIdx > maxTileIdx) {
      continue;
    }
    if (!this->buffer_cache.contains(tileIdx)) {
      const VkDeviceSize tileBufferSize =
          this->infoInfo->width * TILE_HEIGHT * this->infoInfo->channels;
      const auto buffer =
          std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
      ret = buffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                    tileBufferSize);

      void *bufferAddr = buffer->GetMappedAddr();

      const size_t beginRow = tileIdx * TILE_HEIGHT;
      const size_t rowSize = this->infoInfo->width * TILE_HEIGHT;
      for (size_t row = 0; row < TILE_HEIGHT; row++) {
        const void *rowData = this->infoInfo->getRowData(beginRow + row);
        const auto targetAddr = reinterpret_cast<void *>(
            reinterpret_cast<size_t>(bufferAddr) + row * rowSize);
        memcpy(targetAddr, rowData, rowSize);
      }
      if (ret != VK_SUCCESS) {
        Logger() << "Failed to allocate tile buffer!" << std::endl;
        return ret;
      }
    }
  }
  return ret;
}

VkResult HugeImageProcesser::CreateTileBuffersCache(const int tileIdx) const {
  VkResult ret = VK_SUCCESS;

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
  const uint32_t maxTileIdx = this->infoInfo->height / TILE_HEIGHT;
  for (const int tileIdx : tilesIdx) {
    if (tileIdx < 0) {
      continue;
    }
    if (tileIdx > maxTileIdx) {
      continue;
    }
    if (!this->buffer_cache.contains(tileIdx)) {
      Logger() << "Input buffer cache does not exist!" << std::endl;
      return VK_ERROR_UNKNOWN;
    }
    this->inputStorageBuffers[tileIdx] = this->buffer_cache[tileIdx];
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

VkResult HugeImageProcesser::Process(int tileIdx,
                                     const std::shared_ptr<IFilter> &filter) {
  VkResult ret = VK_SUCCESS;
  // TODO: impl me
  return ret;
}

void HugeImageProcesser::Process(const int tileIdx,
                                 const std::shared_ptr<IFilter> &filter,
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

  const VkDeviceSize outputBufferSize =
      output->width * output->height * output->channels;

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

  const uint64_t imageDownloadStart = TimeUtils::GetCurrentMonoMs();
  this->outputBuffer->DownloadData(output->data, outputBufferSize);
  const uint64_t imageDownloadEnd = TimeUtils::GetCurrentMonoMs();
  Logger() << "Image Download Time: " << imageDownloadEnd - imageDownloadStart
           << "ms" << std::endl;
}