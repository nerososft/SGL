//
// Created by neo on 2025/3/14.
//

#ifndef DISSOLVEBLENDER_H
#define DISSOLVEBLENDER_H
#include "effect_engine/blenders/BaseBlender.h"

struct DissolveBlenderParams {
    BasicBlenderParam imageSize;
    float blenderFactor;
};

class DissolveBlender final : public BaseBlender {
    DissolveBlenderParams dissolveBlenderParams{};

public:
    DissolveBlender() = default;

    ~DissolveBlender() override = default;

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   BlendImageInfo baseImageInfo,
                   BlendImageInfo blendImageInfo,
                   VkBuffer outputBuffer) override;

    void SetDissolveFactor(const float dissolveFactor) { this->dissolveBlenderParams.blenderFactor = dissolveFactor; }

    void Destroy() override;
};


#endif //DISSOLVEBLENDER_H
