//
// Created by neo on 2025/3/20.
//

#ifndef VKGPUBUFFER_H
#define VKGPUBUFFER_H
#include "VkGPUContext.h"
#include <memory>

#define KB (1024)
#define MB (KB * KB)
#define MIN_BLOCK_SIZE (64 * KB)
#define MAX_THREADS (8)

typedef enum {
    GPU_BUFFER_TYPE_UNIFORM,
    GPU_BUFFER_TYPE_STORAGE_LOCAL,
    GPU_BUFFER_TYPE_STORAGE_SHARED,
    GPU_BUFFER_TYPE_VERTEX,
    GPU_BUFFER_TYPE_INDEX,
} VkGPUBufferType;

class VkGPUBuffer {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    VkGPUBufferType type;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
    void *data = nullptr;
    VkDeviceSize bufferSize = 0;

public:
    explicit VkGPUBuffer(const std::shared_ptr<VkGPUContext> &gpuCtx);

    ~VkGPUBuffer();

    VkResult AllocateAndBind(VkGPUBufferType type, VkDeviceSize size);

    VkResult MapBuffer(VkDeviceSize size);

    VkResult MapBuffer();

    void UnMapBuffer() const;

    VkResult UploadData(const void *uploadData, VkDeviceSize size);

    VkResult DownloadData(void *downloadAddr, VkDeviceSize size);

    void Destroy();

    [[nodiscard]] VkBuffer GetBuffer() const { return buffer; }

    [[nodiscard]] void *GetMappedAddr() const { return data; }
};

#endif //VKGPUBUFFER_H
