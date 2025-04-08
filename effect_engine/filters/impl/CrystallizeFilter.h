#pragma once
#pragma once
#ifndef CRYSTALLIZEFILTER_H
#define CRYSTALLIZEFILTER_H
#include <vulkan/vulkan_core.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"
#include <effect_engine/gpu/VkGPUBuffer.h>
struct CrystallizeFilterParams {
    BasicFilterParam imageSize;
    int  n;
};
class CrystallizeFilter final : public BasicFilter {
    CrystallizeFilterParams crystallizeFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<VkGPUBuffer> posxBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> posyBuffer = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
    float* posx = nullptr;
    float* posy = nullptr;
    int k_size = 0;

public:
    CrystallizeFilter() = default;

    ~CrystallizeFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext>& gpuCtx,
        std::vector<FilterImageInfo> inputImageInfo,
        std::vector<FilterImageInfo> outputImageInfo) override;;

    void SetPos(float* _posx,float* _posy, int size) {
        posx = _posx;
        posy = _posy;
        k_size = size;
    }
    void SetN(const int n) { this->crystallizeFilterParams.n = n; }
    // void SetK(const int * k) { this->kFilterParams.k = k; }

    void Destroy() override;
};
#endif