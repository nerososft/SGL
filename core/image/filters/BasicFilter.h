//
// Created by 1234 on 2025/3/6.
//

#ifndef BASEFILTER_H
#define BASEFILTER_H
#include <vulkan/vulkan_core.h>

#include "IFilter.h"
#include "runtime/gpu/VkGPUContext.h"
#include "runtime/gpu/compute_graph/ComputeGraph.h"

struct BasicFilterParam {
  uint32_t width;
  uint32_t height;
  uint32_t channels;
  uint32_t bytesPerLine;
};

struct BasicFilterParams {
  size_t paramsSize;
  void *paramsData;
  std::string shaderPath;
};

class BasicFilter : public IFilter {
  std::shared_ptr<ComputeGraph> computeGraph = nullptr;
  std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

public:
  BasicFilter() = default;

  ~BasicFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::string &name, VkDeviceSize bufferSize,
                 VkBuffer inputBuffer, VkBuffer outputBuffer,
                 const BasicFilterParams &filterParams, uint32_t workGroupX,
                 uint32_t workGroupY, uint32_t workGroupZ);

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void Destroy() override;
};

#endif // BASEFILTER_H
