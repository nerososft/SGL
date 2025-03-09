//
// Created by 1234 on 2025/3/6.
//

#ifndef GRAYFILTER_H
#define GRAYFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BaseFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"


class GrayFilter final : public BaseFilter {
public:
    GrayFilter() = default;

    ~GrayFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx, VkDeviceSize bufferSize, uint32_t width,
                   uint32_t height, VkBuffer inputBuffer, VkBuffer outputBuffer) override;
};


#endif //GRAYFILTER_H
