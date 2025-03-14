//
// Created by neo on 2025/3/14.
//

#ifndef BASEBLEND_H
#define BASEBLEND_H
#include "IBlender.h"

class BaseBlender : public IBlender {
public:
    BaseBlender() = default;

    ~BaseBlender() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   BlendImageInfo baseImageInfo,
                   BlendImageInfo blendImageInfo,
                   VkBuffer outputBuffer) override;

    void Destroy() override;
};

#endif //BASEBLEND_H
