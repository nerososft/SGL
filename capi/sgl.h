//
// Created by neo on 25-7-23.
//

#ifndef SGL_H
#define SGL_H
#include "sgl_buffer.h"
#include "sgl_compute.h"
#include "sgl_gpu_ctx.h"
#include "sgl_image.h"

typedef struct sgl {
  sgl_gpu_ctx_t info;
  sgl_buffer_manager_t buffer;
  sgl_image_t img;
  sgl_compute_t cc;
} sgl_t;

sgl_t *sgl_create();

void sgl_destroy(sgl_t *sgl);

#endif // SGL_H
