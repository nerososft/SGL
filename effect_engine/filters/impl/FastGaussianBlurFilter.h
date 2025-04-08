//
// Created by neo on 2025/3/27.
//

#ifndef FASTGAUSSIANBLURFILTER_H
#define FASTGAUSSIANBLURFILTER_H

#define MAX_RADIUS (256)
#include "ScaleFilter.h"
#include "effect_engine/filters/BasicFilter.h"
#include "effect_engine/gpu/VkGPUBuffer.h"

struct FastGaussianBlurFilterParams {
    BasicFilterParam imageSize;
    int radius;
};

class FastGaussianBlurFilter final : public IFilter {
    FastGaussianBlurFilterParams blurFilterParams{};
    ScaleFilterParams scaleDownParams{};
    ScaleFilterParams scaleUpParams{};
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

    std::shared_ptr<VkGPUBuffer> scaleDownBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> scaleDownBlurBuffer = nullptr;

    std::shared_ptr<IComputeGraphNode> CreateScaleDownNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                                           VkBuffer inputBuffer,
                                                           VkDeviceSize inputBufferSize,
                                                           VkBuffer outputBuffer,
                                                           VkDeviceSize outputBufferSize,
                                                           uint32_t targetWidth,
                                                           uint32_t targetHeight);

    std::shared_ptr<IComputeGraphNode> CreateVBlurNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                                       VkBuffer inputBuffer,
                                                       VkDeviceSize inputBufferSize,
                                                       VkBuffer outputBuffer,
                                                       VkDeviceSize outputBufferSize);

    std::shared_ptr<IComputeGraphNode> CreateHBlurNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                                       VkBuffer inputBuffer,
                                                       VkDeviceSize inputBufferSize,
                                                       VkBuffer outputBuffer,
                                                       VkDeviceSize outputBufferSize);

    std::shared_ptr<IComputeGraphNode> CreateScaleUpNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                                         VkBuffer inputBuffer,
                                                         VkDeviceSize inputBufferSize,
                                                         VkBuffer outputBuffer,
                                                         VkDeviceSize outputBufferSize,
                                                         uint32_t targetWidth,
                                                         uint32_t targetHeight);

public:
    FastGaussianBlurFilter() = default;

    ~FastGaussianBlurFilter() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;

    void SetRadius(const int radius) { this->blurFilterParams.radius = radius; }

    void Destroy() override;
};


#endif //FASTGAUSSIANBLURFILTER_H
