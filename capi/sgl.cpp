//
// Created by neo on 25-7-23.
//

#include "include/sgl.h"

#include "runtime/log/Log.h"

sgl_t *sgl_create() {
  const auto sgl = static_cast<sgl_t *>(malloc(sizeof(sgl_t)));
  if (sgl == nullptr) {
    return nullptr;
  }

  sgl_image_t *image = sgl_image_create(&sgl->info);
  if (image == nullptr) {
    Logger() << Logger::ERROR << "Failed to create image engine!" << std::endl;
    sgl_destroy(sgl);
    return nullptr;
  }
  sgl->img = image;

  const sgl_buffer_manager_t *bm = sgl_buffer_manager_create(&sgl->info);
  if (bm == nullptr) {
    Logger() << Logger::ERROR << "Failed to create buffer manager!"
             << std::endl;
    sgl_image_destroy(image);
    sgl_destroy(sgl);
    return nullptr;
  }
  sgl->bm = bm;

  return sgl;
}

void sgl_destroy(sgl_t *sgl) {
  if (sgl == nullptr) {
    return;
  }
  free(sgl);
}
