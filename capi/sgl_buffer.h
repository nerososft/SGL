//
// Created by neo on 25-7-28.
//

#ifndef SGL_BUFFER_H
#define SGL_BUFFER_H
#include "sgl_error.h"
#include "sgl_gpu_ctx.h"

#include <stddef.h>

typedef enum sgl_buffer_type {
  SGL_BUFFER_TYPE_UNIFORM,
  SGL_BUFFER_TYPE_STORAGE_LOCAL,
  SGL_BUFFER_TYPE_STORAGE_SHARED,
  SGL_BUFFER_TYPE_VERTEX,
  SGL_BUFFER_TYPE_INDEX,
  SGL_BUFFER_TYPE_UNKNOWN,
} sgl_buffer_type_t;

typedef struct sgl_buffer {
  sgl_buffer_type_t type = SGL_BUFFER_TYPE_UNKNOWN;
  void *handle = nullptr;
  void *data = nullptr;
  size_t bufferSize = 0;
} sgl_buffer_t;

typedef struct sgl_buffer_manager {
  sgl_buffer_t (*allocate_buf)(sgl_buffer_manager *mgr, size_t size);
  sgl_error_t (*destroy_buf)(sgl_buffer_manager *mgr, sgl_buffer_t *buf);
} sgl_buffer_manager_t;

sgl_buffer_manager_t *sgl_buffer_manager_create(const sgl_gpu_ctx_t *gpu_ctx);

void sgl_buffer_manager_destroy(sgl_buffer_manager_t *bufMgr);

#endif // SGL_BUFFER_H
