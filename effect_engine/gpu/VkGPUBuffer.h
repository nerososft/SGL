//
// Created by neo on 2025/3/20.
//

#ifndef VKGPUBUFFER_H
#define VKGPUBUFFER_H
#include "VkGPUContext.h"

typedef enum {
    GPU_BUFFER_TYPE_UNIFORM,
    GPU_BUFFER_TYPE_STORAGE_LOCAL,
    GPU_BUFFER_TYPE_STORAGE_SHARED,
} VkGPUBufferType;

class VkGPUBuffer {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    VkGPUBufferType type;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
    void *data = nullptr;

public:
    explicit VkGPUBuffer(const std::shared_ptr<VkGPUContext> &gpuCtx);

    ~VkGPUBuffer();

    VkResult AllocateAndBind(VkGPUBufferType type, VkDeviceSize size);

    VkResult MapBuffer(VkDeviceSize size);

    void UnMapBuffer() const;

    VkResult UploadData(const void *uploadData, VkDeviceSize size);

    VkResult DownloadData(void *downloadAddr, VkDeviceSize size);

    void Destroy() const;

    VkBuffer GetBuffer() const { return buffer; }

    void *GetMappedAddr() const { return data; }
};

#endif //VKGPUBUFFER_H
