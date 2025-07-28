//
// Created by neo on 25-7-28.
//
#include "sgl_buffer.h"

#include <cstdlib>

sgl_buffer_manager_t *sgl_buffer_manager_create() {
  const auto bufMgr =
      static_cast<sgl_buffer_manager_t *>(malloc(sizeof(sgl_buffer_manager_t)));
  if (bufMgr == nullptr) {
    return nullptr;
  }

  // TODO:
  return nullptr;
}

void sgl_buffer_manager_destroy(sgl_buffer_manager_t *bufMgr) { free(bufMgr); }