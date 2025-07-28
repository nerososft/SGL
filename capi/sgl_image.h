//
// Created by neo on 25-7-23.
//

#ifndef SGL_IMAGE_H
#define SGL_IMAGE_H
#include "sgl_buffer.h"
#include "sgl_error.h"

#include <cstdint>

typedef struct sgl_image_gpu_info {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t channels = 0;
  uint32_t bytesPerLine = 0;
  sgl_buffer_t gpuBuf{};
} sgl_image_gpu_info_t;

typedef struct sgl_image_cpu_info {
  uint32_t width;
  uint32_t height;
  uint32_t channels;
  uint32_t bytesPerLine;
  void *data;
} sgl_image_cpu_info_t;

typedef enum sgl_image_type {
  SGL_IMAGE_TYPE_CPU = 0,
  SGL_IMAGE_TYPE_GPU = 1,
  SGL_IMAGE_TYPE_UNKNOWN = 2
} sgl_image_type_t;

typedef struct sgl_image_info {
  sgl_image_type_t type = SGL_IMAGE_TYPE_UNKNOWN;
  union {
    sgl_image_gpu_info_t gpu;
    sgl_image_cpu_info_t cpu;
  } info;
} sgl_image_info_t;

typedef struct sgl_image {
  sgl_error_t (*gray)(sgl_image_info_t in, sgl_image_info_t out, const float r,
                      const float g, const float b);
} sgl_image_t;

sgl_image_t *sgl_image_create();

void sgl_image_destroy(sgl_image_t *image);

#endif // SGL_IMAGE_H