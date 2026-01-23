//
// Created by neo on 2026/1/22.
//

#ifndef SGL_TILEBASEDFASTGAUSSIANBLURFILTER_H
#define SGL_TILEBASEDFASTGAUSSIANBLURFILTER_H

#include "../ITileBasedFilter.h"
#include "TileBasedScaleFilter.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/VkGPUContext.h"
#include "runtime/gpu/compute_graph/ComputeGraph.h"

#include <memory>
#include <vulkan/vulkan_core.h>

struct FastGaussianBlurFilterParams {
  TileBasedFilterParam imageSize;
  int radius;
};

class TileBasedFastGaussianBlurFilter : public ITileBasedFilter {
  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

  std::vector<FilterImageInfo> inputImageInfo;
  std::vector<FilterImageInfo> vBlurImageInfo;
  std::vector<FilterImageInfo> hBlurImageInfo;
  std::vector<FilterImageInfo> scaleUpImageInfo;

  FastGaussianBlurFilterParams gaussianFilterParams{};
  FilterParams filterParams{};

  TiledScaleFilterParams scaleDownFilterParams{};
  FilterParams scaleDownParams{};
  TiledScaleFilterParams scaleUpFilterParams{};
  FilterParams scaleUpParams{};

  std::shared_ptr<VkGPUBuffer> scaleDownBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> scaleDownBlurBuffer = nullptr;
  std::shared_ptr<VkGPUBuffer> outputBuffer = nullptr;

  std::shared_ptr<IComputeGraphNode> CreateTileBasedScaleDownNode(
      const std::vector<FilterImageInfo> &inputImageInfo, uint32_t targetWidth,
      uint32_t targetHeight);

  std::shared_ptr<IComputeGraphNode>
  CreateTileBasedVBlurNode(const std::vector<FilterImageInfo> &inputImageInfo,
                           uint32_t targetWidth, uint32_t targetHeight,
                           const std::string &shaderPath);

  std::shared_ptr<IComputeGraphNode>
  CreateTileBasedHBlurNode(const std::vector<FilterImageInfo> &inputImageInfo,
                           uint32_t targetWidth, uint32_t targetHeight,
                           const std::string &shaderPath);

  std::shared_ptr<IComputeGraphNode>
  CreateTileBasedScaleUpNode(const std::vector<FilterImageInfo> &inputImageInfo,
                             uint32_t targetWidth, uint32_t targetHeight,
                             VkBuffer outputBuffer,
                             VkDeviceSize outputBufferSize);

public:
  TileBasedFastGaussianBlurFilter() = default;

  ~TileBasedFastGaussianBlurFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx, int tileIdx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void SetRadius(const int radius) {
    this->gaussianFilterParams.radius = radius;
  }

  void Destroy() override;
};

#endif // SGL_TILEBASEDFASTGAUSSIANBLURFILTER_H
