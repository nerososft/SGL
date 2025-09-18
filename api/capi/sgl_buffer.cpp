//
// Created by neo on 25-7-28.
//
#include "include/sgl/sgl_buffer.h"

#include "core/context/Context.h"
#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/log/Log.h"

#include <map>

std::map<VkBuffer, std::shared_ptr<VkGPUBuffer>> buffer_map;

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
  buffer_map.emplace(buffer->GetBuffer(), buffer);
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
  if (buf->bufHandle == VK_NULL_HANDLE) {
    Logger() << Logger::ERROR << "Attempting to destroy null buffer!"
             << std::endl;
    return SGL_INVALID_ARGUMENT;
  }
  if (!buffer_map.contains(static_cast<VkBuffer>(buf->bufHandle))) {
    Logger() << Logger::ERROR << "Attempting to destroy non-existent buffer!"
             << std::endl;
    return SGL_INVALID_ARGUMENT;
  }
  const auto buffer =
      buffer_map.find(static_cast<VkBuffer>(buf->bufHandle))->second;
  buffer->Destroy();
  buffer_map.erase(static_cast<VkBuffer>(buf->bufHandle));
  buf->type = SGL_BUFFER_TYPE_UNKNOWN;
  buf->bufHandle = VK_NULL_HANDLE;
  buf->memHandle = VK_NULL_HANDLE;
  buf->data = nullptr;
  buf->bufferSize = 0;
  return SGL_SUCCESS;
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