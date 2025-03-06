//
// Created by 1234 on 2025/3/6.
//

#include "EffectEngine.h"

#include <iostream>
#include <ostream>

bool EffectEngine::Init() {
    std::vector<const char *> requiredExtensions;
    std::vector<const char *> enabledExtensions;
    gpuCtx = std::make_shared<VKGPUContext>();
    if (!gpuCtx->Init(requiredExtensions, enabledExtensions)) {
        std::cout << "Failed to initialize GPU context" << std::endl;
        return false;
    };
}

void EffectEngine::Process(const char *inputFilePath, const char *outputFilePath,
    const std::shared_ptr<GrayFilter> &filter) {
}
