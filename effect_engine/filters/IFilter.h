//
// Created by neo on 2025/3/9.
//

#ifndef IFILTER_H
#define IFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/gpu/VkGPUContext.h"

class IFilter {
public:
    IFilter() = default;

    virtual ~IFilter() = default;

    virtual VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                           VkDeviceSize bufferSize,
                           uint32_t width,
                           uint32_t height,
                           VkBuffer inputBuffer,
                           VkBuffer outputBuffer) = 0;
};

#endif //IFILTER_H
