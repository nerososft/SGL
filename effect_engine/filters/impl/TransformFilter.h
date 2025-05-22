//
// Created by 1234 on 2025/3/6.
//

#ifndef TRANSFORMFILTER_H
#define TRANSFORMFILTER_H
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

#include "effect_engine/filters/BasicFilter.h"
#include "gpu_engine/gpu/VkGPUContext.h"


struct TransformFilterParams {
    BasicFilterParam imageSize;
    glm::mat4 transformMatrix;
};

class TransformFilter final : public BasicFilter {
    TransformFilterParams transformFilterParams{};

public:
    TransformFilter() = default;

    ~TransformFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetTransformMatrix(const glm::mat4 matrix) { this->transformFilterParams.transformMatrix = matrix; }

    void Destroy() override;
};


#endif //TRANSFORMFILTER_H
