//
// Created by neo on 2025/3/14.
//

#ifndef DISSOLVEBLENDER_H
#define DISSOLVEBLENDER_H
#include "effect_engine/blend/BaseBlender.h"


class DissolveBlender final : public BaseBlender {
public:
    DissolveBlender() = default;

    ~DissolveBlender() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   BlendImageInfo baseImageInfo,
                   BlendImageInfo blendImageInfo,
                   VkBuffer outputBuffer) override;

    void Destroy() override;
};


#endif //DISSOLVEBLENDER_H
