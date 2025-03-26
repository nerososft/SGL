//
// Created by 1234 on 2025/3/6.
//

#ifndef COLORBALANCEFILTER_H
#define COLORBALANCEFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"
#include <effect_engine/gpu/VkGPUBuffer.h>
struct colorBalanceFilterParams {
    BasicFilterParam imageSize;
    int preserve_luminosity;
};

class colorBalanceFilter final : public BasicFilter {
    colorBalanceFilterParams bFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<VkGPUBuffer> adjustPBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> PBuffer = nullptr;
    int* P = nullptr;
    float* adjustP = nullptr;
    int pSize = 0;
    int adjustPSize = 0;

public:
    colorBalanceFilter() = default;

    ~colorBalanceFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetP(int * _p , int _size) {
        P = _p;
        pSize = _size;
    }
    void SetAdjustP(float* _adjust_p, int _size) {
        adjustP = _adjust_p;
        adjustPSize = _size;
    }

    void SetL(int l) {
        bFilterParams.preserve_luminosity = l;
    }

    void Destroy() override;
};


#endif //GRAYFILTER_H
