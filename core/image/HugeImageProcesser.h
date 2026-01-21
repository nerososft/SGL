//
// Created by neo on 2026/1/20.
//

#ifndef SGL_HUGEIMAGEENGINE_H
#define SGL_HUGEIMAGEENGINE_H
#include "runtime/gpu/VkGPUBuffer.h"
#include "sgl/sgl_image.h"
#include "tile/TileBasedFilter.h"

#include <map>
#include <memory>

#define TILE_HEIGHT (256)
#define PROCESS_TILES_BUFFER_CNT (9)

using HugeImageInfo = sgl_image_huge_cpu_info_t;
using ImageInfoCpu = sgl_image_cpu_info_t;

class HugeImageProcesser {
  std::shared_ptr<HugeImageInfo> infoInfo;

  std::map<int, std::shared_ptr<VkGPUBuffer>> buffer_cache; // 9 buffers

  std::vector<std::shared_ptr<VkGPUBuffer>> inputStorageBuffers;

  std::shared_ptr<VkGPUBuffer> outputBuffer;

  VkResult CreateTileBufferCache(int tileIdx);

  [[nodiscard]] VkResult
  CheckAndCreateTilesBuffer(const std::vector<int> &tilesIdx);

  [[nodiscard]] VkResult CreateTileBuffersCache(int tileIdx);

  VkResult CheckAndPrepareInputBuffers(const std::vector<int> &tilesIdx);

  VkResult PrepareInputBufferForTile(int tileIdx);

  [[nodiscard]] VkResult
  Process(int tileIdx, const std::shared_ptr<TileBasedFilter> &filter) const;

public:
  explicit HugeImageProcesser(const std::shared_ptr<HugeImageInfo> &input);

  ~HugeImageProcesser() = default;

  [[nodiscard]] VkResult Init() const;

  void Process(int tileIdx, const std::shared_ptr<TileBasedFilter> &filter,
               const std::shared_ptr<ImageInfoCpu> &output);

  [[nodiscard]] std::shared_ptr<VkGPUBuffer> GetOutputBuffer(int tileIdx) const;
};

#endif // SGL_HUGEIMAGEENGINE_H
