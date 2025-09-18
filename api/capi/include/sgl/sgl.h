//
// Created by neo on 25-7-23.
//

#ifndef SGL_H
#define SGL_H
#include "sgl_buffer.h"
#include "sgl_compute.h"
#include "sgl_gpu_ctx.h"
#include "sgl_image.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  const sgl_gpu_ctx_t info;
  const sgl_buffer_manager_t *bm;
  const sgl_image_t *img;
  const sgl_compute_t *cc;
} sgl_t;

sgl_t *sgl_create();

void sgl_destroy(sgl_t *sgl);

#ifdef __cplusplus
}
#endif

#endif // SGL_H
