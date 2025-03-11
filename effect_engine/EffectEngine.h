//
// Created by 1234 on 2025/3/6.
//

#ifndef EFFECTENGINE_H
#define EFFECTENGINE_H

#include "filters/impl/GrayFilter.h"
#include "gpu/VkGPUContext.h"

struct ImageInfo {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint32_t bytesPerLine;
    void *data;
};

class EffectEngine {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;

public:
    EffectEngine() = default;

    ~EffectEngine() = default;

    bool Init();

    VkResult Process(VkBuffer *inputStorageBuffer,
                     VkDeviceMemory *inputStorageBufferMemory,
                     uint32_t width,
                     uint32_t height,
                     uint32_t channels, const void *uploadData, VkBuffer *outputStorageBuffer,
                     VkDeviceMemory *outputStorageBufferMemory, const
                     std::shared_ptr<IFilter> &filter) const;

    void Process(const ImageInfo &input,
                 const ImageInfo &output,
                 const std::shared_ptr<GrayFilter> &filter) const;

    void Process(const char *inputFilePath,
                 const char *outputFilePath,
                 const std::shared_ptr<GrayFilter> &filter) const;
};


#endif //EFFECTENGINE_H
