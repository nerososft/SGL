//
// Created by neo on 25-7-7.
//

#ifndef YUVTORGBFILTER_H
#define YUVTORGBFILTER_H
#include <vulkan/vulkan_core.h>

#include "GrayFilter.h"
#include "engine/effect/filters/BasicFilter.h"


struct YuvToRgbFilterParams {
    BasicFilterParam imageSize;
    uint32_t format;
};

class YuvToRgbFilter final : public BasicFilter {
    GrayFilterParams yuvToRgbFilterParams{};

public:
    YuvToRgbFilter() = default;

    ~YuvToRgbFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void Destroy() override;
};

#endif //YUVTORGBFILTER_H
