//
// Created by neo on 25-4-30.
//


#include <memory>

#include "gpu_engine/log/Log.h"
#include "renderer/Renderer.h"

int main(int argc, char *argv[]) {
    Logger() << "Hello World!\n";

    std::shared_ptr<Renderer> renderer = nullptr;

    renderer = std::make_shared<Renderer>(768, 768);
    const std::vector<const char *> requiredExtensions;
    if (!renderer->Init(requiredExtensions)) {
        Logger() << "Failed to initialize renderer" << std::endl;
    }
    renderer->RenderFrameOffScreen("../../renderer_demo/images/render_offscreen1.png");
    return 0;
}
