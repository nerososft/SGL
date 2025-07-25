//
// Created by neo on 2025/3/12.
//

#ifndef COLORPENCILSKETCHFILTER_H
#define COLORPENCILSKETCHFILTER_H
#include "engine/image/filters/BasicFilter.h"

struct ColorPencilSketchFilterParams {
  BasicFilterParam imageSize;
};

class ColorPencilSketchFilter final : public BasicFilter {
  ColorPencilSketchFilterParams colorPencilSketchFilterParams{};

public:
  ColorPencilSketchFilter() = default;

  ~ColorPencilSketchFilter() override = default;

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void Destroy() override;
};

#endif // COLORPENCILSKETCHFILTER_H
