//
// Created by neo on 25-4-3.
//

#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H
#include <effect_engine/gpu/compute_graph/ComputeGraph.h>

#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/filters/IFilter.h"

struct MedianFilterParams {
    BasicFilterParam imageSize;
    uint32_t piece;
    uint32_t pieceCount;
    float radius;
};

class MedianFilter final : public IFilter {
    std::vector<MedianFilterParams> medianFilterParams;
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    float radius = 0.0f;

public:
    MedianFilter() = default;

    ~MedianFilter() override = default;

    std::shared_ptr<SubComputeGraph> CreateParallelSubGraph(size_t parallelIndex,
                                                            const std::shared_ptr<VkGPUContext> &gpuCtx,
                                                            VkBuffer inputBuffer,
                                                            VkDeviceSize inputBufferSize,
                                                            VkBuffer outputBuffer,
                                                            VkDeviceSize outputBufferSize);

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   std::vector<FilterImageInfo> inputImageInfo,
                   std::vector<FilterImageInfo> outputImageInfo) override {
        return VK_SUCCESS;
    }

    void SetRadius(const float radius) {
        this->radius = radius;
    }

    void Destroy() override;
};


#endif //MEDIANFILTER_H
