//
// Created by neo on 25-7-23.
//

#ifndef SGL_IMAGE_H
#define SGL_IMAGE_H
#include "sdl_error.h"

#include <cstdint>

typedef struct sgl_image_info {
  uint32_t width;
  uint32_t height;
  uint32_t channels;
  uint32_t bytesPerLine;
  void *data;
} sgl_image_info_t;

typedef struct sgl_image {
  sgl_error_t (*gray)(sgl_image_info_t in, sgl_image_info_t out, const float r,
                      const float g, const float b);
} sgl_image_t;

sgl_image_t *sgl_image_create();

void sgl_image_destroy(sgl_image_t *image);

#endif // SGL_IMAGE_H