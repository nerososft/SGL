//
// Created by 1234 on 2025/3/6.
//

#ifndef THREADHOLDFILTER
#define THREADHOLDFILTER
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"

struct thresholdSplitFilterParams {
    BasicFilterParam imageSize;
    int bright;
};

class thresholdSplitFilter final : public BasicFilter {
    thresholdSplitFilterParams grayFilterParams{};

public:
    thresholdSplitFilter() = default;

    ~thresholdSplitFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetBright(const int bright) { this->grayFilterParams.bright = bright; }

    void Destroy() override;
};


#endif //GRAYFILTER_H
