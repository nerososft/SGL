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
    uint32_t inputWidthStride;
    uint32_t inputHeightStride;
    uint32_t format; // 0: I420
};

class YuvToRgbFilter final : public BasicFilter {
    YuvToRgbFilterParams yuvToRgbFilterParams{};

    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

public:
    YuvToRgbFilter() = default;

    ~YuvToRgbFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetInputWidthStride(const uint32_t inputWidthStride) {
        this->yuvToRgbFilterParams.inputWidthStride = inputWidthStride;
    }

    void SetInputHeightStride(const uint32_t inputHeightStride) {
        this->yuvToRgbFilterParams.inputHeightStride = inputHeightStride;
    }

    void SetFormat(const uint32_t format) { this->yuvToRgbFilterParams.format = format; }

    void Destroy() override;
};

#endif //YUVTORGBFILTER_H
