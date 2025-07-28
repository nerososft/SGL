//
// Created by neo on 25-7-28.
//

#include "sgl_compute.h"
#include "runtime/log/Log.h"

sgl_compute_t *sgl_buffer_manager_create(const sgl_gpu_ctx_t *gpu_ctx) {
  if (gpu_ctx == nullptr) {
    Logger() << "gpu_ctx is null." << std::endl;
    return nullptr;
  }
  const auto compute =
      static_cast<sgl_compute_t *>(malloc(sizeof(sgl_compute_t)));
  if (compute == nullptr) {
    return nullptr;
  }
  // TODO:

  return compute;
}

void sgl_buffer_manager_destroy(sgl_compute_t *compute) { free(compute); }