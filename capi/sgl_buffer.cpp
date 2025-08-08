//
// Created by neo on 25-7-28.
//
#include "include/sgl_buffer.h"

#include "core/context/Context.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/log/Log.h"

sgl_buffer_t sgl_buffer_manager_allocate_buffer(const sgl_buffer_manager *mgr,
                                                sgl_buffer_type type,
                                                const size_t size) {
  if (size == 0) {
    Logger() << Logger::ERROR << "Can not allocate 0 bytes gpu mem!"
             << std::endl;
    sgl_buffer_t buf{};
    buf.type = SGL_BUFFER_TYPE_UNKNOWN;
    return buf;
  }
  const auto buffer =
      std::make_shared<VkGPUBuffer>(Context::GetInstance()->GetContext());
  VkResult ret =
      buffer->AllocateAndBind(static_cast<VkGPUBufferType>(type), size);
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to allocate gpu buffer!" << std::endl;
    sgl_buffer_t buf{};
    buf.type = SGL_BUFFER_TYPE_UNKNOWN;
    return buf;
  }
  ret = buffer->MapBuffer(size);
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to map gpu buffer!" << std::endl;
    buffer->Destroy();
    sgl_buffer_t buf{};
    buf.type = SGL_BUFFER_TYPE_UNKNOWN;
    return buf;
  }
  Logger() << "Allocated gpu buffer " << string_buffer_type(type)
           << " size: " << size << std::endl;
  sgl_buffer_t buf;
  buf.type = type;
  buf.bufferSize = size;
  buf.bufHandle = buffer->GetBuffer();
  buf.memHandle = buffer->GetDeviceMemory();
  buf.data = buffer->GetMappedAddr();
  return buf;
}

sgl_error_t sgl_buffer_manager_destroy_buffer(const sgl_buffer_manager *mgr,
                                              sgl_buffer_t *buf) {
  // TODO:
  return (sgl_error_t){};
}

sgl_buffer_manager_t *sgl_buffer_manager_create(const sgl_gpu_ctx_t *gpu_ctx) {
  if (gpu_ctx == nullptr) {
    Logger() << Logger::ERROR << "gpu_ctx is null." << std::endl;
    return nullptr;
  }
  const auto bufMgr =
      static_cast<sgl_buffer_manager_t *>(malloc(sizeof(sgl_buffer_manager_t)));
  if (bufMgr == nullptr) {
    Logger() << Logger::ERROR << "Failed to allocate sgl_buffer_manager_t."
             << std::endl;
    return nullptr;
  }

  bufMgr->allocate_buffer = sgl_buffer_manager_allocate_buffer;
  bufMgr->destroy_buffer = sgl_buffer_manager_destroy_buffer;
  return bufMgr;
}

void sgl_buffer_manager_destroy(sgl_buffer_manager_t *bufMgr) { free(bufMgr); }