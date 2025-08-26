//
// Created by neo on 25-7-25.
//

#ifndef SGL_COMPUTE_H
#define SGL_COMPUTE_H
#include "sgl_buffer.h"
#include "sgl_error.h"
#include "sgl_gpu_ctx.h"
#include <stddef.h>

typedef struct sgl_tensor {
  size_t ndim;
  size_t *shape;
  sgl_buffer_t gpuBuf;
} sgl_tensor_t;

typedef struct sgl_compute {
  sgl_tensor_t *(*tensor_create)(sgl_buffer_manager_t *buf_mgr, size_t ndim,
                                 size_t *shape, void *data);
  sgl_error_t (*tensor_destroy)(sgl_buffer_manager_t *buf_mgr,
                                sgl_tensor_t *tensor);
  sgl_error_t (*mat_mul)(sgl_tensor_t *mat1, sgl_tensor_t *mat2,
                         sgl_tensor_t *out);
  sgl_error_t (*record)();
  sgl_error_t (*eval)(struct sgl_compute *cc);
  // TODO: more func
} sgl_compute_t;

sgl_compute_t *sgl_compute_create(sgl_gpu_ctx_t *gpu_ctx);

void sgl_compute_destroy(sgl_compute_t *compute);

#endif // SGL_COMPUTE_H
