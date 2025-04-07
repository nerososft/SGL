//
// Created by neo on 2025/3/14.
//

#ifndef BASEBLEND_H
#define BASEBLEND_H
#include "IBlender.h"
#include "effect_engine/gpu/compute_graph/ComputeGraph.h"

struct BasicBlenderParam {
    uint32_t baseImageWidth;
    uint32_t baseImageHeight;
    uint32_t baseImageChannels;
    uint32_t baseImageBytesPerLine;

    uint32_t blendImageWidth;
    uint32_t blendImageHeight;
    uint32_t blendImageChannels;
    uint32_t blendImageBytesPerLine;
    uint32_t blendImagePosX;
    uint32_t blendImagePosY;
};

struct BasicBlenderParams {
    size_t paramsSize;
    void *paramsData;
    std::string shaderPath;
};

class BaseBlender : public IBlender {
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;

public:
    BaseBlender() = default;

    ~BaseBlender() override = default;

    VkResult DoApply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                     const std::string &name,
                     const BlendImageInfo &baseImageInfo,
                     const BlendImageInfo &blendImageInfo,
                     VkBuffer outputBuffer,
                     const BasicBlenderParams& blenderParams,
                     uint32_t workGroupX,
                     uint32_t workGroupY,
                     uint32_t workGroupZ);

    VkResult DoApply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                     const std::string &name,
                     const BlendImageInfo &baseImageInfo,
                     const BlendImageInfo &blendImageInfo,
                     VkBuffer outputBuffer,
                     const BasicBlenderParams &blenderParams);

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   BlendImageInfo baseImageInfo,
                   BlendImageInfo blendImageInfo,
                   VkBuffer outputBuffer) override;

    void Destroy() override;
};

#endif //BASEBLEND_H
