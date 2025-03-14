//
// Created by neo on 2025/3/14.
//

#include "DissolveBlender.h"

VkResult DissolveBlender::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                BlendImageInfo baseImageInfo,
                                BlendImageInfo blendImageInfo,
                                VkBuffer outputBuffer) {
}

void DissolveBlender::Destroy() {
    BaseBlender::Destroy();
}
