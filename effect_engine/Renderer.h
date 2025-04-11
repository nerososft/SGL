//
// Created by neo on 25-4-11.
//

#ifndef RENDERER_H
#define RENDERER_H
#include "gpu/VkGPUContext.h"

class Renderer {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;

public:
    Renderer() = default;

    bool Init();

    void RenderFrame();

    ~Renderer() = default;
};

#endif //RENDERER_H
