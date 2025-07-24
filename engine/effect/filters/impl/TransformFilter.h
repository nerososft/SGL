//
// Created by neo on 25-6-18.
//

#ifndef TRANSFORMFILTER_H
#define TRANSFORMFILTER_H

#include <glm/glm.hpp>

#include "engine/effect/filters/BasicFilter.h"

struct TransformFilterParams {
  BasicFilterParam imageSize;
  glm::mat4 transformMatrix;
};

class TransformFilter final : public BasicFilter {
  TransformFilterParams transformFilterParams{};

public:
  TransformFilter() = default;

  ~TransformFilter() override = default;

  void SetTransformMatrix(const glm::mat4 &transformMatrix) {
    this->transformFilterParams.transformMatrix = transformMatrix;
  }

  VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::vector<FilterImageInfo> &inputImageInfo,
                 const std::vector<FilterImageInfo> &outputImageInfo) override;

  void Destroy() override;
};

#endif // TRANSFORMFILTER_H
