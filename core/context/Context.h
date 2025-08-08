//
// Created by neo on 25-8-7.
//

#ifndef CONTEXT_H
#define CONTEXT_H
#include "runtime/gpu/VkGPUContext.h"

#include <memory>

class Context {
  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;

  bool Init();

public:
  Context() = default;
  ~Context() = default;

  static std::shared_ptr<Context> GetInstance();

  std::shared_ptr<VkGPUContext> GetContext() { return gpuCtx; }
};

#endif // CONTEXT_H
