//
// Created by 1234 on 2025/3/6.
//

#ifndef IMAGENGINE_H
#define IMAGENGINE_H
#include "blenders/IBlender.h"
#include "filters/IFilter.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/VkGPUContext.h"
#include <string>

namespace sgl::image {
struct ImageInfo {
  uint32_t width;
  uint32_t height;
  uint32_t channels;
  uint32_t bytesPerLine;
  void *data;
};

class ImageEngine {
  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;

  VkResult Process(const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                   uint32_t inputWidth, uint32_t inputHeight,
                   uint32_t outputWidth, uint32_t outputHeight,
                   uint32_t channels, const void *uploadData,
                   const std::shared_ptr<VkGPUBuffer> &outputBuffer,
                   const std::shared_ptr<IFilter> &filter) const;

public:
  ImageEngine() = default;

  ~ImageEngine() = default;

  bool Init();

  [[nodiscard]] std::string GetGPUName() const;

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
