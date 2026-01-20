//
// Created by neo on 2026/1/20.
//

#ifndef SGL_HUGEIMAGEENGINE_H
#define SGL_HUGEIMAGEENGINE_H
#include "filters/IFilter.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "sgl/sgl_image.h"

#include <memory>
#include <unordered_map>

#define TILE_HEIGHT (256)
#define PROCESS_TILES_BUFFER_CNT (9)

using HugeImageInfo = sgl_image_huge_cpu_info_t;
using ImageInfoCpu = sgl_image_cpu_info_t;

class HugeImageProcesser {
  std::shared_ptr<HugeImageInfo> infoInfo;

  std::unordered_map<int, std::shared_ptr<VkGPUBuffer>>
      buffer_cache; // 9 buffers

  std::vector<std::shared_ptr<VkGPUBuffer>> inputStorageBuffers;

  std::shared_ptr<VkGPUBuffer> outputBuffer;

  [[nodiscard]] VkResult
  CheckAndCreateTilesBuffer(const std::vector<int> &tilesIdx) const;

  [[nodiscard]] VkResult CreateTileBuffersCache(int tileIdx) const;

  VkResult CheckAndPrepareInputBuffers(const std::vector<int> &tilesIdx);

  VkResult PrepareInputBufferForTile(int tileIdx);

  VkResult Process(int tileIdx, const std::shared_ptr<IFilter> &filter);

public:
  explicit HugeImageProcesser(const std::shared_ptr<HugeImageInfo> &input);

  [[nodiscard]] VkResult Init() const;

  ~HugeImageProcesser() = default;

  void Process(int tileIdx, const std::shared_ptr<IFilter> &filter,
               const std::shared_ptr<ImageInfoCpu> &output);
};

#endif // SGL_HUGEIMAGEENGINE_H
