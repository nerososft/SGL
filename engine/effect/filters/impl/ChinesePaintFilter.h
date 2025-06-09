//
// Created by 1234 on 2025/3/6.
//

#ifndef CHINESE_PAINT_FILTER_H
#define CHINESE_PAINT_FILTER_H
#include <vulkan/vulkan_core.h>

#include "engine/effect/filters/BasicFilter.h"
#include "core/gpu/VkGPUContext.h"

struct ChinesePaintFilterParams {
    BasicFilterParam imageSize;
};

class ChinesePaintFilter final : public BasicFilter {
    ChinesePaintFilterParams chinesePaintFilterParams{};

public:
    ChinesePaintFilter() = default;

    ~ChinesePaintFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void Destroy() override;
};


#endif //CHINESE_PAINT_FILTER_H
