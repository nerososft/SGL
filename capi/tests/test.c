//
// Created by neo on 25-8-11.
//

#include <sgl/sgl.h>
#include <sgl/sgl_buffer.h>
#include <sgl/sgl_image.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  const sgl_t *gl = sgl_create();
  if (gl == NULL) {
    printf("sgl_create failed\n");
    return -1;
  }

  const size_t width = 1024;
  const size_t height = 768;
  const size_t channels = 4;
  const sgl_buffer_t input_buffer = gl->bm->allocate_buffer(
      gl->bm, SGL_BUFFER_TYPE_STORAGE_SHARED, width * height * channels);
  if (input_buffer.type == SGL_BUFFER_TYPE_UNKNOWN) {
    printf("Failed to allocate buffer\n");
    return -1;
  }

  const size_t target_width = 640;
  const size_t target_height = 480;
  const sgl_buffer_t output_buffer =
      gl->bm->allocate_buffer(gl->bm, SGL_BUFFER_TYPE_STORAGE_SHARED,
                              target_width * target_height * channels);
  if (output_buffer.type == SGL_BUFFER_TYPE_UNKNOWN) {
    printf("Failed to allocate buffer\n");
    return -1;
  }

  const sgl_image_info_t input = {.type = SGL_IMAGE_TYPE_GPU,
                                  .info = {.gpu = {.width = width,
                                                   .height = height,
                                                   .channels = channels,
                                                   .gpuBuf = input_buffer}}};

  const sgl_image_info_t output = {.type = SGL_IMAGE_TYPE_GPU,
                                   .info = {.gpu = {.width = target_width,
                                                    .height = target_height,
                                                    .channels = channels,
                                                    .gpuBuf = output_buffer}}};

  const sgl_error_t err =
      gl->img->scale(input, output, target_width, target_height, 1);
  if (err != SGL_SUCCESS) {
    printf("Scale failed\n");
    return -1;
  }
  return 0;
}