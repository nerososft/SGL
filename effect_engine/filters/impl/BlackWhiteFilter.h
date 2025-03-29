//
// Created by 1234 on 2025/3/6.
//

#ifndef BLACKWHITEFILTER
#define BLACKWHITEFILTER
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"
#include <effect_engine/gpu/VkGPUBuffer.h>
struct BlackWhiteFilterParams {
    BasicFilterParam imageSize;

};

class BlackWhiteFilter final : public BasicFilter {
    BlackWhiteFilterParams wFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<VkGPUBuffer> weightBuffer = nullptr;

    float* weight = nullptr;
    int wSize;

public:
    BlackWhiteFilter() = default;

    ~BlackWhiteFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetWeight(float * _weight , int _wSize) {
        weight = _weight;
        wSize = _wSize;
    }

    void Destroy() override;
};


#endif //GRAYFILTER_H
