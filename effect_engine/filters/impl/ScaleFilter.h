//
// Created by neo on 2025/3/13.
//

#ifndef SCALEFILTER_H
#define SCALEFILTER_H
#include "effect_engine/filters/BasicFilter.h"

struct ScaleFilterParams {
    BasicFilterParam imageSize;
    uint32_t targetWidth;
    uint32_t targetHeight;
};

class ScaleFilter final : public BasicFilter {
    ScaleFilterParams scaleFilterParams{};

public:
    ScaleFilter() = default;

    ~ScaleFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void Destroy() override;

    void SetTargetWidth(const uint32_t width) { this->scaleFilterParams.targetWidth = width; }
    void SetTargetHeight(const uint32_t height) { this->scaleFilterParams.targetHeight = height; }
};


#endif //SCALEFILTER_H
