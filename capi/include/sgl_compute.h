//
// Created by neo on 25-7-25.
//

#ifndef SGL_COMPUTE_H
#define SGL_COMPUTE_H
#include "sgl_gpu_ctx.h"
#include "sgl_error.h"

typedef struct sgl_tensor {
} sgl_tensor_t;

typedef struct sgl_compute {
  sgl_error_t (*mat_mul)(sgl_tensor_t mat1, sgl_tensor_t mat2,
                         sgl_tensor_t *out);
  // TODO: more func
} sgl_compute_t;

sgl_compute_t *sgl_compute_create(sgl_gpu_ctx_t *gpu_ctx);

void sgl_compute_destroy(sgl_compute_t *image);

#endif // SGL_COMPUTE_H
