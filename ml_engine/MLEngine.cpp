//
// Created by neo on 25-5-7.
//

#include "MLEngine.h"

#include "gpu_engine/log/Log.h"

bool MLEngine::Init() {
    std::vector<const char *> requiredExtensions;
    this->gpuCtx = std::make_shared<VkGPUContext>(requiredExtensions);
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_validation");
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_LUNARG_api_dump");
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_synchronization2");
    // this->gpuCtx->AddDeviceEnabledExtension("VK_KHR_synchronization2");
    VkResult result = this->gpuCtx->Init();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to initialize Vulkan GPU context!" << std::endl;
        return false;
    }
    Logger() << Logger::INFO << "Initialized Compute Engine, version: " << VERSION << std::endl;

    this->mainSubGraph = std::make_shared<SubComputeGraph>(this->gpuCtx);
    if (this->mainSubGraph == nullptr) {
        Logger() << Logger::ERROR << "Failed to initialize SubComputeGraph!" << std::endl;
        return false;
    }
    result = this->mainSubGraph->Init();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to initialize main SubComputeGraph!" << std::endl;
        return false;
    }

    this->graph = std::make_shared<ComputeGraph>(this->gpuCtx);
    if (this->graph == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph!" << std::endl;
        return false;
    }
    this->graph->AddSubGraph(this->mainSubGraph);

    return true;
}

MLEngine MLEngine::ReLU(std::vector<float> input, std::vector<float> output) {
    return *this;
}

void MLEngine::Compute() const {
    const VkResult result = this->graph->Compute();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to compute graph!" << std::endl;
    }
}
