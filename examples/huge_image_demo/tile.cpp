//
// Created by neoyang on 2025/3/6.
//

#include "core/image/HugeImageProcessor.h"
#include "core/image/tile_filters/impl/TileBasedGrayFilter.h"
#include "core/utils/ImageUtils.h"
#include "runtime/log/Log.h"
#include <memory>
#include <ostream>
#include <vulkan/vulkan_core.h>

std::vector<char> imageData;
uint32_t imageWidth, imageHeight, channel;

void *ReadPngRow(const size_t row) {
  auto addr = static_cast<void *>(imageData.data());
  const size_t rowSize = imageWidth * channel;
  const auto targetAddr =
      reinterpret_cast<void *>(reinterpret_cast<size_t>(addr) + row * rowSize);
  // Logger() << "ReadRow:" << row << ", addr:" << targetAddr << std::endl;
  return targetAddr;
}

void FreeRow(void *row) {}

void effect_engine_main() {
  imageData =
      ImageUtils::ReadPngFile("../../../examples/image_demo/images/girl.png",
                              &imageWidth, &imageHeight, &channel);

  const auto &imageInfo = std::make_shared<HugeImageInfo>();
  imageInfo->width = imageWidth;
  imageInfo->height = imageHeight;
  imageInfo->channels = channel;
  imageInfo->getRowData = &ReadPngRow;
  imageInfo->afterRowDataUse = &FreeRow;

  const auto processor = std::make_shared<HugeImageProcessor>(imageInfo);

  const auto filter = std::make_shared<TileBasedGrayFilter>();
  filter->SetRedFactor(0.299);
  filter->SetGreenFactor(0.587);
  filter->SetBlueFactor(0.114);
  if (processor->Init() != VK_SUCCESS) {
    Logger() << "Failed to init processor" << std::endl;
    return;
  }

  void *data = malloc(imageWidth * channel * TILE_HEIGHT);
  if (data == nullptr) {
    Logger() << "Failed to allocate memory for data" << std::endl;
    return;
  }

  const size_t tileCount = imageHeight / TILE_HEIGHT;
  for (int i = 0; i < tileCount; i++) {
    const int tileIdx = i;
    auto info = std::make_shared<ImageInfoCpu>();
    info->width = imageWidth;
    info->height = TILE_HEIGHT;
    info->channels = channel;
    info->data = data;
    processor->Process(tileIdx, filter, info);
    std::string path = "../../../examples/huge_image_demo/images/girl";
    path.append(std::to_string(tileIdx));
    path.append(".png");
    ImageUtils::WritePngFile(path, imageWidth, TILE_HEIGHT, channel, data);
  }
}

int main(int argc, char *argv[]) {
  Logger() << "Hello World!\n";
  effect_engine_main();
  // getchar();
  return 0;
}
