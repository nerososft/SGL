//
// Created by neo on 25-8-7.
//

#include "Context.h"

#include "runtime/log/Log.h"

static std::shared_ptr<Context> instance = nullptr;
std::shared_ptr<Context> Context::GetInstance() {
  if (instance == nullptr) {
    const auto ctx = std::make_shared<Context>();
    if (!ctx->Init()) {
      return nullptr;
    }
    instance = ctx;
  }
  return instance;
}

bool Context::Init() {
  if (this->gpuCtx == nullptr) {
    std::vector<const char *> requiredExtensions;
    this->gpuCtx = std::make_shared<VkGPUContext>(requiredExtensions);
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_validation");
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_LUNARG_api_dump");
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_synchronization2");
    // this->gpuCtx->AddDeviceEnabledExtension("VK_KHR_synchronization2");
  }
  if (this->gpuCtx->Init() != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to initialize Vulkan GPU context!"
             << std::endl;
    return false;
  }
  Logger() << Logger::INFO << "Initialized ImageEngine, version: " << VERSION
           << std::endl;
  return true;
}