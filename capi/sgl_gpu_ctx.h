//
// Created by neo on 25-7-28.
//

#ifndef SGL_GPUCTX_H
#define SGL_GPUCTX_H

typedef struct sgl_gpu_ctx {
  void *physicalDevice = nullptr;
  void *logicalDevice = nullptr;
  void *queue = nullptr;

  const char *(*get_gpu_name)();
} sgl_gpu_ctx_t;

#endif // SGL_GPUCTX_H
