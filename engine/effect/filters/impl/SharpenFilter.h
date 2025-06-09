#pragma once
#ifndef SHARPENFILTER_H
#define SHARPENFILTER_H
#include <vulkan/vulkan_core.h>

#include "engine/effect/filters/BasicFilter.h"
#include "core/gpu/VkGPUContext.h"
#include <core/gpu/VkGPUBuffer.h>

struct SharpenFilterParams {
    BasicFilterParam imageSize;
};

class SharpenFilter final : public IFilter {
    SharpenFilterParams sharpenFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
    std::shared_ptr<VkGPUBuffer> kernelBuffer = nullptr;
    int *kernel = nullptr;
    int size = 0;

public:
    SharpenFilter() = default;

    ~SharpenFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;;

    void SetKernel(int *_kernel, const int size) {
        this->kernel = _kernel;
        this->size = size;
    }

    void Destroy() override;
};
#endif
