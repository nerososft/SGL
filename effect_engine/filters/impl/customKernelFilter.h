//
// Created by 1234 on 2025/3/6.
//

#ifndef KFILTER_H
#define KFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"
#include <effect_engine/gpu/VkGPUBuffer.h>
struct customKernelFilterParams {
    BasicFilterParam imageSize;
    int  radius;
    int  scale;
    int offset;
};

class customKernelFilter final : public BasicFilter {
    customKernelFilterParams kFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<VkGPUBuffer> kBuffer = nullptr;
    int* k = nullptr;
    int k_size = 0;

public:
    customKernelFilter() = default;

    ~customKernelFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetK(int *_k , int size) {
        k = _k;
        k_size = size;
    }
    void SetRadius(const int radius) { this->kFilterParams.radius = radius; }
    void SetScale(const int scale) { this->kFilterParams.scale = scale; }
    void SetOffset(const int offset) { this->kFilterParams.offset = offset; }
   // void SetK(const int * k) { this->kFilterParams.k = k; }

    void Destroy() override;
};


#endif //GRAYFILTER_H
