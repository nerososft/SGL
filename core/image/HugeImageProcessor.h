//
// Created by neo on 2026/1/20.
//

#ifndef SGL_HUGEIMAGEENGINE_H
#define SGL_HUGEIMAGEENGINE_H
#include "runtime/gpu/VkGPUBuffer.h"
#include "sgl/sgl_image.h"
#include "tile_filters/ITileBasedFilter.h"

#include <array>
#include <map>
#include <memory>
#include <future>
#include <mutex>
#include <queue>

#define TILE_HEIGHT (256)
#define PROCESS_TILES_BUFFER_CNT (9)

using HugeImageInfo = sgl_image_huge_cpu_info_t;
using ImageInfoCpu = sgl_image_cpu_info_t;

class HugeImageProcessor {
  std::shared_ptr<HugeImageInfo> infoInfo;

  std::vector<std::shared_ptr<VkGPUBuffer>> freeBufferList;

  std::map<int, std::shared_ptr<VkGPUBuffer>> buffer_cache; // 9 buffers

  std::vector<std::shared_ptr<VkGPUBuffer>> inputStorageBuffers;

  std::shared_ptr<VkGPUBuffer> tilePlaceholderBuffer;
  std::array<std::shared_ptr<VkGPUBuffer>, 2> outputBuffers{};
  std::future<VkResult> prefetchFuture;
  int prefetchedTileIdx = -1;
  mutable std::mutex bufferCacheMutex;

  [[nodiscard]] std::shared_ptr<VkGPUBuffer> GetBufferFromFreeList();
  void ReturnBufferToFreeList(const std::shared_ptr<VkGPUBuffer> &buffer);
  [[nodiscard]] int GetMaxTileIdx() const;
  [[nodiscard]] int GetOutputSlot(int tileIdx) const;
  void WaitForPrefetch();
  void StartPrefetchForNextTile(int currentTileIdx);

  VkResult CreateTileBufferCache(int tileIdx);

  [[nodiscard]] VkResult
  CheckAndCreateTilesBuffer(const std::vector<int> &tilesIdx);

  [[nodiscard]] VkResult CreateTileBuffersCache(int tileIdx);

  VkResult CheckAndPrepareInputBuffers(const std::vector<int> &tilesIdx);

  VkResult PrepareInputBufferForTile(int tileIdx);

  [[nodiscard]] VkResult
  Process(int tileIdx, const std::shared_ptr<ITileBasedFilter> &filter) const;

public:
  explicit HugeImageProcessor(const std::shared_ptr<HugeImageInfo> &input);

  ~HugeImageProcessor() = default;

  [[nodiscard]] VkResult Init() const;

  void Destroy();

  [[nodiscard]] VkResult
  PrepareOutputBuffer(const std::shared_ptr<ImageInfoCpu> &output);

  void *Process(int tileIdx, const std::shared_ptr<ITileBasedFilter> &filter,
                const std::shared_ptr<ImageInfoCpu> &output);

  [[nodiscard]] std::shared_ptr<VkGPUBuffer> GetOutputBuffer(int tileIdx) const;
};

#endif // SGL_HUGEIMAGEENGINE_H
