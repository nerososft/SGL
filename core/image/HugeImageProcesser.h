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

  std::unordered_map<size_t, VkGPUBuffer> buffer_cache; // 9 buffers

  std::vector<std::shared_ptr<VkGPUBuffer>> inputStorageBuffers;

  std::shared_ptr<VkGPUBuffer> outputBuffer;

  VkResult PrepareInputBufferForTile(size_t tileIdx);

  VkResult Process(size_t tileIdx, const std::shared_ptr<IFilter> &filter);

public:
  explicit HugeImageProcesser(const std::shared_ptr<HugeImageInfo> &input);

  VkResult Init() const;

  ~HugeImageProcesser() = default;

  void Process(size_t tileIdx, const std::shared_ptr<IFilter> &filter,
               const std::shared_ptr<ImageInfoCpu> &output);
};

#endif // SGL_HUGEIMAGEENGINE_H
