//
// Created by 1234 on 2025/3/6.
//

#include "EffectEngine.h"

#include <iostream>
#include <ostream>

bool EffectEngine::Init() {
    std::vector<const char *> requiredExtensions;
    std::vector<const char *> enabledExtensions;
    this->gpuCtx = std::make_shared<VkGPUContext>(requiredExtensions);
    const VkResult result = this->gpuCtx->Init();
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to initialize Vulkan GPU context!" << std::endl;
        return false;
    }
}

void EffectEngine::Process(const char *inputFilePath, const char *outputFilePath,
                           const std::shared_ptr<GrayFilter> &filter) {
}
