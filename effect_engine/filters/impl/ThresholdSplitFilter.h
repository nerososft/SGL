//
// Created by 1234 on 2025/3/6.
//

#ifndef THREADHOLDFILTER
#define THREADHOLDFILTER
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"

struct ThresholdSplitFilterParams {
    BasicFilterParam imageSize;
    int bright;
};

class ThresholdSplitFilter final : public BasicFilter {
    ThresholdSplitFilterParams thresholdSplitFilterParams{};

public:
    ThresholdSplitFilter() = default;

    ~ThresholdSplitFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetBright(const int bright) { this->thresholdSplitFilterParams.bright = bright; }

    void Destroy() override;
};


#endif //THREADHOLDFILTER
