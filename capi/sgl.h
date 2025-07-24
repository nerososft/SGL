//
// Created by neo on 25-7-23.
//

#ifndef SGL_H
#define SGL_H
#include <cstdint>

typedef enum sgl_error {
  SGL_SUCCESS = 0,
  SGL_FAILURE = 1,
  SGL_VULKAN_ERROR = 2,
  SGL_OUT_OF_MEMORY = 3,
} sgl_error_t;

typedef struct sgl_info {
  const char *(*get_gpu_name)();
  // TODO: more func
} sgl_info_t;

typedef struct sgl_image_info {
  uint32_t width;
  uint32_t height;
  uint32_t channels;
  uint32_t bytesPerLine;
  void *data;
} sgl_image_info_t;

typedef struct sgl_image {
  sgl_error_t (*gray)(sgl_image_info_t in, sgl_image_info_t out);
} sgl_image_t;

typedef struct sgl_tensor {

} sgl_tensor_t;

typedef struct sgl_compute {
  sgl_error_t (*mat_mul)(sgl_tensor_t mat1, sgl_tensor_t mat2,
                         sgl_tensor_t *out);
  // TODO: more func
} sgl_compute_t;

typedef struct sgl {
  sgl_info_t info;
  sgl_image_t img;
  sgl_compute_t cc;
} sgl_t;

sgl_t *sgl_create();

void sgl_destroy(sgl_t *sgl);

#endif // SGL_H
