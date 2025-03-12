//
// Created by neo on 2025/3/12.
//

#ifndef COLORPENCILSKETCHFILTER_H
#define COLORPENCILSKETCHFILTER_H
#include "effect_engine/filters/BasicFilter.h"

struct ColorPencilSketchFilterParams {
    BasicFilterParam imageSize;
};

class ColorPencilSketchFilter final : public BasicFilter {
    ColorPencilSketchFilterParams colorPencilSketchFilterParams{};

public:
    ColorPencilSketchFilter() = default;

    ~ColorPencilSketchFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void Destroy() override;
};

#endif //COLORPENCILSKETCHFILTER_H
