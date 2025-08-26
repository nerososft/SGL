//
// Created by neo on 25-7-28.
//

#include "include/sgl/sgl_compute.h"
#include "runtime/log/Log.h"

sgl_tensor_t *sgl_tensor_create(const sgl_buffer_manager_t *buf_mgr,
                                const size_t ndim, size_t *shape,
                                const void *data) {
  if (buf_mgr == nullptr) {
    Logger() << "buffer manager is null" << std::endl;
    return nullptr;
  }
  if (shape == nullptr) {
    Logger() << "shape is nullptr" << std::endl;
    return nullptr;
  }
  const auto tensor = static_cast<sgl_tensor_t *>(malloc(sizeof(sgl_tensor_t)));
  if (tensor == nullptr) {
    Logger() << "malloc failed" << std::endl;
    return nullptr;
  }
  tensor->ndim = ndim;
  tensor->shape = shape;
  size_t totalSize = 0;
  for (size_t i = 0; i < ndim; ++i) {
    totalSize += shape[i];
  }
  if (totalSize <= 0) {
    Logger() << "totalSize is 0" << std::endl;
    free(tensor);
    return nullptr;
  }

  const auto buf = buf_mgr->allocate_buffer(
      buf_mgr, SGL_BUFFER_TYPE_STORAGE_SHARED, totalSize);
  if (buf.type == SGL_BUFFER_TYPE_UNKNOWN) {
    Logger() << "gpu mem allocated failed" << std::endl;
    free(tensor);
    return nullptr;
  }
  tensor->gpuBuf = buf;

  if (data != nullptr) {
    memcpy(buf.data, data, totalSize);
  }
  return tensor;
}

sgl_error_t sgl_tensor_destroy(const sgl_buffer_manager_t *buf_mgr,
                               sgl_tensor_t *tensor) {
  if (tensor == nullptr) {
    Logger() << "tensor is nullptr" << std::endl;
    return SGL_INVALID_ARGUMENT;
  }
  if (buf_mgr == nullptr) {
    Logger() << "buf manager is null" << std::endl;
    return SGL_INVALID_ARGUMENT;
  }

  const sgl_error_t err = buf_mgr->destroy_buffer(buf_mgr, &tensor->gpuBuf);
  if (err != SGL_SUCCESS) {
    Logger() << "destroy buffer failed" << std::endl;
    return err;
  }
  free(tensor);
  return SGL_SUCCESS;
}

sgl_compute_t *sgl_compute_create(const sgl_gpu_ctx_t *gpu_ctx) {
  if (gpu_ctx == nullptr) {
    Logger() << "gpu_ctx is null." << std::endl;
    return nullptr;
  }
  const auto compute =
      static_cast<sgl_compute_t *>(malloc(sizeof(sgl_compute_t)));
  if (compute == nullptr) {
    return nullptr;
  }
  // TODO:

  return compute;
}

void sgl_compute_destroy(sgl_compute_t *compute) { free(compute); }