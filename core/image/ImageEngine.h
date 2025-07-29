//
// Created by 1234 on 2025/3/6.
//

#ifndef IMAGENGINE_H
#define IMAGENGINE_H
#include "../../capi/include/sgl_image.h"
#include "blenders/IBlender.h"
#include "filters/IFilter.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/VkGPUContext.h"
#include <string>

namespace sgl::image {
using ImageInfo = sgl_image_info_t;
using ImageInfoCpu = sgl_image_cpu_info_t;
using ImageInfoGpu = sgl_image_gpu_info_t;
using BufferGpu = sgl_buffer_t;

class ImageEngine {
  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;

  VkResult Process(const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                   uint32_t inputWidth, uint32_t inputHeight,
                   uint32_t outputWidth, uint32_t outputHeight,
                   uint32_t channels, const void *uploadData,
                   const std::shared_ptr<VkGPUBuffer> &outputBuffer,
                   const std::shared_ptr<IFilter> &filter) const;

  void ProcessFromCpuAddr(const ImageInfoCpu &input, const ImageInfoCpu &output,
                          const std::shared_ptr<IFilter> &filter) const;

  void ProcessFromGpuAddr(const ImageInfoGpu &input, const ImageInfoGpu &output,
                          const std::shared_ptr<IFilter> &filter) const;

  void ProcessFromCpuAddr(const std::vector<ImageInfoCpu> &inputs,
                          const std::vector<ImageInfoCpu> &outputs,
                          const std::shared_ptr<IFilter> &filter) const;

public:
  ImageEngine() = default;

  explicit ImageEngine(const std::shared_ptr<VkGPUContext> &gpuCtx);

  ~ImageEngine() = default;

  bool Init();

  [[nodiscard]] std::string GetGPUName() const;

  [[nodiscard]] VkResult
  ProcessGpu(const BufferGpu &inputBuffer, uint32_t inputWidth,
             uint32_t inputHeight, const BufferGpu &outputBuffer,
             uint32_t outputWidth, uint32_t outputHeight, uint32_t channels,
             const std::shared_ptr<IFilter> &filter) const;

  void Process(const ImageInfo &input, const ImageInfo &output,
               const std::shared_ptr<IFilter> &filter) const;

  void Process(const std::vector<ImageInfo> &inputs,
               const std::vector<ImageInfo> &outputs,
               const std::shared_ptr<IFilter> &filter) const;

  void Process(const char *inputFilePath, const char *outputFilePath,
               const std::shared_ptr<IFilter> &filter) const;

  void Process(const char *inputFilePath, const char *outputFilePath,
               uint32_t newWidth, uint32_t newHeight,
               const std::shared_ptr<IFilter> &filter) const;

  void Process(const char *baseFilePath, const char *blendFilePath,
               uint32_t posX, uint32_t posY, const char *outputFilePath,
               const std::shared_ptr<IBlender> &blender) const;
};
} // namespace sgl::image

#endif // IMAGENGINE_H
