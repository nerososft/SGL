#pragma once
#ifndef TWIRLWARPFILTER_H
#define TWIRLWARPFILTER_H
#include <vulkan/vulkan_core.h>

#include "engine/effect/filters/BasicFilter.h"
#include "core/gpu/VkGPUContext.h"

struct TwirlWarpFilterParams {
    BasicFilterParam imageSize;
    int angle;
};

class TwirlWarpFilter final : public BasicFilter {
    TwirlWarpFilterParams twirlwarpFilterParams{};

public:
    TwirlWarpFilter() = default;

    ~TwirlWarpFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetAngle(const int angle) { this->twirlwarpFilterParams.angle = angle; }

    void Destroy() override;
};
#endif
