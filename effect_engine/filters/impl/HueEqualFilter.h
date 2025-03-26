//
// Created by 1234 on 2025/3/6.
//

#ifndef HUEFILTER_H
#define HUEFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"

struct HueEqualFilterParams {
    BasicFilterParam imageSize;
    float redFactor;
    float greenFactor;
    float blueFactor;
};

class HueEqualFilter final : public BasicFilter {
    HueEqualFilterParams hueFilterParams{};

public:
    HueEqualFilter() = default;

    ~HueEqualFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;


    void Destroy() override;
};


#endif //GRAYFILTER_H
