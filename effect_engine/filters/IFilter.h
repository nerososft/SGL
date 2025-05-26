//
// Created by neo on 2025/3/9.
//

#ifndef IFILTER_H
#define IFILTER_H
#include <vulkan/vulkan_core.h>

#include "gpu_engine/gpu/VkGPUContext.h"

#include <memory>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint32_t posX;
    uint32_t posY;
    VkDeviceSize bufferSize;
    VkBuffer storageBuffer;
    VkDeviceMemory storageBufferMemory;
} FilterImageInfo;


class IFilter {
public:
    IFilter() = default;

    virtual ~IFilter() = default;

    virtual VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           const std::vector<FilterImageInfo> &inputImageInfo,
                           const std::vector<FilterImageInfo> &outputImageInfo) = 0;

    virtual void Destroy() = 0;
};

#endif //IFILTER_H
