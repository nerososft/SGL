//
// Created by 1234 on 2025/3/6.
//

#ifndef VKHELPER_H
#define VKHELPER_H
#include <vector>
#include <vulkan/vulkan_core.h>


class VkGPUHelper {
public:
    VkGPUHelper() = default;

    ~VkGPUHelper() = default;

    static VkResult CreateBuffer(VkDevice device, VkDeviceSize size,
                                 VkBufferUsageFlagBits usage,
                                 VkSharingMode sharingMode,
                                 const std::vector<uint32_t> &queueFamilyIndices,
                                 VkBuffer *buffer);

    static uint32_t GetRequiredMemTypeIndex(const VkPhysicalDeviceMemoryProperties *memProps,
                                            const VkMemoryRequirements &memRequirements,
                                            uint32_t memoryPropertyFlagsBits);

    static VkResult CreateStorageBufferAndBindMem(VkDevice device, VkDeviceSize size,
                                                  const std::vector<uint32_t> &queueFamilyIndices,
                                                  const VkPhysicalDeviceMemoryProperties *memProps,
                                                  VkBuffer *storageBuffer,
                                                  VkDeviceMemory *storageBufferMemory);
};


#endif //VKHELPER_H
