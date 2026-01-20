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

VkResult HugeImageProcesser::PrepareInputBufferForTile(size_t tileIdx) {
  VkResult ret = VK_SUCCESS;
  // TODO: impl me
  return ret;
}

VkResult HugeImageProcesser::Process(size_t tileIdx,
                                     const std::shared_ptr<IFilter> &filter) {
  VkResult ret = VK_SUCCESS;
  // TODO: impl me
  return ret;
}
void HugeImageProcesser::Process(const size_t tileIdx,
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