//
// Created by neo on 25-7-8.
//

#ifndef RGBTOYUVFILTER_H
#define RGBTOYUVFILTER_H
#include <engine/effect/filters/BasicFilter.h>

struct RgbToYuvFilterParams {
    BasicFilterParam imageSize;
    uint32_t format;
};

class RgbToYuvFilter final : public BasicFilter {
    RgbToYuvFilterParams rgbToYuvFilterParams{};

public:
    RgbToYuvFilter() = default;

    ~RgbToYuvFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void Destroy() override;
};

#endif //RGBTOYUVFILTER_H
