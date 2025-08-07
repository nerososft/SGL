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

  const sgl_image_t *image = sgl_image_create(&sgl->info);
  if (image == nullptr) {
    Logger() << Logger::ERROR << "Failed to create image engine!" << std::endl;
    free(sgl);
    return nullptr;
  }
  sgl->img = image;

  // TODO:
  return sgl;
}

void sgl_destroy(sgl_t *sgl) {
  if (sgl == nullptr) {
    return;
  }
  free(sgl);
}
