//
// Created by neo on 25-4-3.
//

#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H
#include <effect_engine/filters/BasicFilter.h>
#include <effect_engine/filters/IFilter.h>
#include <effect_engine/gpu/compute_graph/ComputeGraph.h>

struct MedianFilterParams {
    BasicFilterParam imageSize;
    int radius;
};

class MedianFilter final : public BasicFilter {
    MedianFilterParams medianFilterParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;

public:
    MedianFilter() = default;

    ~MedianFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetRadius(const int radius) { this->medianFilterParams.radius = radius; }

    void Destroy() override;
};



#endif //MEDIANFILTER_H
