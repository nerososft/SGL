//
// Created by neo on 2025/3/20.
//

#include "VkGPUBuffer.h"

#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "VkGPUHelper.h"
#include "effect_engine/log/Log.h"
#include <thread>
#include <algorithm>

VkResult VkGPUBuffer::AllocateAndBind(const VkGPUBufferType type, const VkDeviceSize size) {
    std::vector<uint32_t> queueFamilyIndices;
    queueFamilyIndices.push_back(0);
    const VkPhysicalDeviceMemoryProperties memoryProperties = gpuCtx->GetMemoryProperties();
    this->type = type;
    VkResult result = VK_SUCCESS;
    if (type == GPU_BUFFER_TYPE_UNIFORM) {
        result = VkGPUHelper::CreateUniformBufferAndBindMem(gpuCtx->GetCurrentDevice(),
                                                            size,
                                                            queueFamilyIndices,
                                                            &memoryProperties,
                                                            &buffer,
                                                            &bufferMemory);
    } else {
        if (type == GPU_BUFFER_TYPE_STORAGE_SHARED) {
            result = VkGPUHelper::CreateStorageBufferAndBindMem(gpuCtx->GetCurrentDevice(),
                                                                size,
                                                                queueFamilyIndices,
                                                                &memoryProperties,
                                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                                &buffer,
                                                                &bufferMemory);
        } else if (type == GPU_BUFFER_TYPE_STORAGE_LOCAL) {
            result = VkGPUHelper::CreateStorageBufferAndBindMem(gpuCtx->GetCurrentDevice(),
                                                                size,
                                                                queueFamilyIndices,
                                                                &memoryProperties,
                                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                                &buffer,
                                                                &bufferMemory);
        }
    }
    if (result != VK_SUCCESS) {
        Logger() << "Failed to allocate buffer and bind, err ="
                << string_VkResult(result)
                << std::endl;
        return result;
    }
    this->bufferSize = size;
    return result;
}

VkResult VkGPUBuffer::MapBuffer(const VkDeviceSize size) {
    if (bufferMemory == VK_NULL_HANDLE) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    if (this->type != GPU_BUFFER_TYPE_UNIFORM && this->type != GPU_BUFFER_TYPE_STORAGE_SHARED) {
        return VK_ERROR_MEMORY_MAP_FAILED;
    }
    const VkResult result = vkMapMemory(this->gpuCtx->GetCurrentDevice(), bufferMemory, 0, size, 0, &data);
    if (result != VK_SUCCESS) {
        Logger() << "Failed to map buffer memory, err =" << string_VkResult(result) << std::endl;
        return result;
    }
    return result;
}

VkResult VkGPUBuffer::MapBuffer() {
    return this->MapBuffer(this->bufferSize);
}

void VkGPUBuffer::UnMapBuffer() const {
    if (bufferMemory == VK_NULL_HANDLE) {
        return;
    }
    if (this->type != GPU_BUFFER_TYPE_UNIFORM && this->type != GPU_BUFFER_TYPE_STORAGE_SHARED) {
        return;
    }
    vkUnmapMemory(this->gpuCtx->GetCurrentDevice(), bufferMemory);
}

VkGPUBuffer::VkGPUBuffer(const std::shared_ptr<VkGPUContext> &gpuCtx): type(GPU_BUFFER_TYPE_STORAGE_SHARED) {
    this->gpuCtx = gpuCtx;
}

VkGPUBuffer::~VkGPUBuffer() {
}

VkResult VkGPUBuffer::UploadData(const void *uploadData, const VkDeviceSize size) {
    const VkResult result = MapBuffer(size);
    if (result != VK_SUCCESS) {
        Logger() << "Failed to map buffer, err =" << string_VkResult(result) << std::endl;
        return result;
    }
    memcpy(data, uploadData, size);
    UnMapBuffer();
    return VK_SUCCESS;
}




void parallel_copy(void* dst, const void* src, size_t size, int num_threads) {
    size_t block_size = size / num_threads;
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; i++) {
        size_t offset = i * block_size;
        size_t remain = (i == num_threads - 1) ? size - offset : block_size;
        threads.emplace_back([=] {
            memcpy((char*)dst + offset, (const char*)src + offset, remain);
            });
    }
    for (auto& t : threads) t.join();
}



VkResult VkGPUBuffer::DownloadData(void *downloadAddr, const VkDeviceSize size) {
    const VkResult result = MapBuffer(size);

    int buffer_size = 9000000;
    if (result != VK_SUCCESS) {
        Logger() << "Failed to map buffer, err =" << string_VkResult(result) << std::endl;
        return result;
    }
    if (size < buffer_size) {
        memcpy(downloadAddr, data, size);
    }
    else {
        parallel_copy(downloadAddr, data, size, 8);
    }

    UnMapBuffer();
    return VK_SUCCESS;
}

void VkGPUBuffer::Destroy() const {
    if (bufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(gpuCtx->GetCurrentDevice(), bufferMemory, nullptr);
    }
    if (buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(gpuCtx->GetCurrentDevice(), buffer, nullptr);
    }
}
