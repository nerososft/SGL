//
// Created by 1234 on 2025/3/6.
//

#ifndef CHINESE_PAINT_FILTER_H
#define CHINESE_PAINT_FILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"

struct ChinesePaintFilterParams {
    BasicFilterParam imageSize;
};

class ChinesePaintFilter final : public BasicFilter {
    ChinesePaintFilterParams chinsesPaintFilterParams{};

public:
    ChinesePaintFilter() = default;

    ~ChinesePaintFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;
};


#endif //CHINESE_PAINT_FILTER_H
