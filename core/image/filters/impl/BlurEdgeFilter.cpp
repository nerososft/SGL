//
// Created by neo on 2025/3/11.
//

#include "BlurEdgeFilter.h"

#ifdef OS_OPEN_HARMONY
#include <runtime/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif
#include "core/image/filters/BasicFilter.h"
#include "runtime/config.h"
#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/gpu/compute_graph/BufferCopyNode.h"
#include "runtime/log/Log.h"

VkResult
BlurEdgeFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                      const std::vector<FilterImageInfo> &inputImageInfo,
                      const std::vector<FilterImageInfo> &outputImageInfo) {
  BasicFilterParams params;
  this->blurEdgeFilterParams.imageSize.width = inputImageInfo[0].width;
  this->blurEdgeFilterParams.imageSize.height = inputImageInfo[0].height;
  this->blurEdgeFilterParams.imageSize.channels = 4;
  this->blurEdgeFilterParams.imageSize.bytesPerLine =
      this->blurEdgeFilterParams.imageSize.width * 4;
  params.paramsSize = sizeof(BlurEdgeFilterParams);
  params.paramsData = &this->blurEdgeFilterParams;
  params.shaderPath = SHADER(blur_edge.comp.glsl.spv);

  return BasicFilter::Apply(gpuCtx, "BlurEdge", inputImageInfo[0].bufferSize,
                            inputImageInfo[0].storageBuffer,
                            outputImageInfo[0].storageBuffer, params,
                            (outputImageInfo[0].width + 31) / 32,
                            (outputImageInfo[0].height + 31) / 32, 1);
}

void BlurEdgeFilter::Destroy() { BasicFilter::Destroy(); }
