#pragma once
#ifndef TILTSHIFTBLURFILTER_H
#define TILTSHIFTBLURFILTER_H
#include <vulkan/vulkan_core.h>

#include "engine/effect/filters/BasicFilter.h"
#include "core/gpu/VkGPUContext.h"
#include <core/gpu/VkGPUBuffer.h>

struct TiltshiftBlurFilterParams {
    BasicFilterParam imageSize;
    float xoffset;
    float yoffset;
};

class TiltshiftBlurFilter final : public IFilter {
    TiltshiftBlurFilterParams tiltshiftblurFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
    std::shared_ptr<VkGPUBuffer> ABuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> BBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> CBuffer = nullptr;
    float *A = nullptr;
    float *B = nullptr;
    float *C = nullptr;
    int size = 0;

public:
    TiltshiftBlurFilter() = default;

    ~TiltshiftBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetA(float *_A, const int size) {
        this->A = _A;
        this->size = size;
    }

    void SetB(float *_B, const int size) {
        this->B = _B;
    }

    void SetC(float *_C, const int size) {
        this->C = _C;
    }

    void SetOffset(const float xoffset, const float yoffset) {
        this->tiltshiftblurFilterParams.xoffset = xoffset;
        this->tiltshiftblurFilterParams.yoffset = yoffset;
    }

    void Destroy() override;
};

#endif //GRAYFILTER_H
