//
// Created by neo on 25-7-28.
//
#include "include/sgl_buffer.h"

#include "runtime/log/Log.h"

#include <cstdlib>

sgl_buffer_t sgl_buffer_manager_allocate_buf(sgl_buffer_manager *mgr,
                                             size_t size) {
  // TODO:
  return (sgl_buffer_t){};
}
sgl_error_t sgl_buffer_manager_destroy_buf(sgl_buffer_manager *mgr,
                                           sgl_buffer_t *buf) {
  // TODO:
  return (sgl_error_t){};
}

sgl_buffer_manager_t *sgl_buffer_manager_create(const sgl_gpu_ctx_t *gpu_ctx) {
  if (gpu_ctx == nullptr) {
    Logger() << "gpu_ctx is null." << std::endl;
    return nullptr;
  }
  const auto bufMgr =
      static_cast<sgl_buffer_manager_t *>(malloc(sizeof(sgl_buffer_manager_t)));
  if (bufMgr == nullptr) {
    return nullptr;
  }

  bufMgr->allocate_buf = sgl_buffer_manager_allocate_buf;
  bufMgr->destroy_buf = sgl_buffer_manager_destroy_buf;
  return bufMgr;
}

void sgl_buffer_manager_destroy(sgl_buffer_manager_t *bufMgr) { free(bufMgr); }