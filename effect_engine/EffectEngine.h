//
// Created by 1234 on 2025/3/6.
//

#ifndef EFFECTENGINE_H
#define EFFECTENGINE_H
#include <memory>

#include "filters/impl/GrayFilter.h"
#include "gpu/VKGPUContext.h"


class EffectEngine {
    std::shared_ptr<VKGPUContext> gpuCtx = nullptr;

public:
    EffectEngine() = default;

    ~EffectEngine() = default;

    bool Init();

    void Process(const char *inputFilePath, const char *outputFilePath, const std::shared_ptr<GrayFilter> &filter);
};


#endif //EFFECTENGINE_H
