//
// Created by neoyang on 2025/3/6.
//

#include "core/image/HugeImageProcesser.h"
#include "core/utils/ImageUtils.h"
#include "runtime/log/Log.h"

#include <memory>
#include <ostream>
#include <vulkan/vulkan_core.h>

void *ReadPngRow(const size_t row) {
  Logger() << "ReadPngRow: " << row << std::endl;
  return ImageUtils::ReadPngFileRow(
      "../../../examples/image_demo/images/girl.png", row);
}

void FreeRow(void *row) {
  free(row);
  Logger() << "FreeRow: " << row << std::endl;
}

void effect_engine_main() {
  uint32_t imageWidth, imageHeight, channel;
  ImageUtils::ReadPngFileSize("../../../examples/image_demo/images/girl.png",
                              &imageWidth, &imageHeight, &channel);

  const auto &imageInfo = std::make_shared<HugeImageInfo>();
  imageInfo->width = imageWidth;
  imageInfo->height = imageHeight;
  imageInfo->channels = channel;
  imageInfo->getRowData = &ReadPngRow;
  imageInfo->afterRowDataUse = &FreeRow;

  const auto processor = std::make_shared<HugeImageProcesser>(imageInfo);

  const auto filter = std::make_shared<TileBasedFilter>();
  if (processor->Init() != VK_SUCCESS) {
    Logger() << "Failed to init processor" << std::endl;
    return;
  }

  void *data = malloc(imageWidth * channel * 256);
  if (data == nullptr) {
    Logger() << "Failed to allocate memory for data" << std::endl;
    return;
  }
  for (int i = 0; i < 1; i++) {
    auto info = std::make_shared<ImageInfoCpu>();
    info->width = imageWidth;
    info->height = 256;
    info->channels = channel;
    info->data = data;
    processor->Process(0, filter, info);
  }
}

int main(int argc, char *argv[]) {
  Logger() << "Hello World!\n";
  effect_engine_main();
  // getchar();
  return 0;
}
