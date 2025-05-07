#include <iostream>
#include <ostream>

#include "gpu_engine/log/Log.h"
#include "ml_engine/operators/IOperator.h"
#include "ml_engine/operators/impl/ReLUOperator.h"
//
// Created by neo on 25-5-7.
//
int main(int argc, char *argv[]) {
    std::cout << "Inference demo" << std::endl;
    std::vector<const char *> extensions;
    const auto gpuCtx = std::make_shared<VkGPUContext>(extensions);
    gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_validation");
    const VkResult result = gpuCtx->Init();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to initialize Vulkan GPU context!" << std::endl;
        return false;
    }

    std::shared_ptr<VkGPUBuffer> inputBuffer;
    std::shared_ptr<VkGPUBuffer> outputBuffer;

    std::shared_ptr<IOperator> relu = std::make_shared<ReLUOperator>(gpuCtx, inputBuffer, outputBuffer);
    // TODO:
}
