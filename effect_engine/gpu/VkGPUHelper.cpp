//
// Created by 1234 on 2025/3/6.
//

#include "VkGPUHelper.h"

#include <iostream>
#include <ostream>
#include <vulkan/vk_enum_string_helper.h>

VkResult VkGPUHelper::CreateBuffer(const VkDevice device,
                                   const VkDeviceSize size,
                                   const VkBufferUsageFlagBits usage,
                                   const VkSharingMode sharingMode,
                                   const std::vector<uint32_t> &queueFamilyIndices,
                                   VkBuffer *buffer) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.flags = 0;
    bufferInfo.pNext = nullptr;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = sharingMode;
    bufferInfo.queueFamilyIndexCount = queueFamilyIndices.size();
    bufferInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    const VkResult ret = vkCreateBuffer(device, &bufferInfo, nullptr, buffer);
    if (ret != VK_SUCCESS) {
        std::cout << "vkCreateBuffer failed, err=" << string_VkResult(ret) << std::endl;
    }
    return ret;
}

uint32_t VkGPUHelper::GetRequiredMemTypeIndex(const VkPhysicalDeviceMemoryProperties *memProps,
                                              const VkMemoryRequirements &memRequirements,
                                              const uint32_t memoryPropertyFlagsBits) {
    for (uint32_t i = 0; i < memProps->memoryTypeCount; i++) {
        if (memRequirements.memoryTypeBits & (1 << i)) {
            if (memoryPropertyFlagsBits & memProps->memoryTypes[i].propertyFlags == memoryPropertyFlagsBits) {
                return i;
            }
        }
    }
    return 0;
}

VkResult VkGPUHelper::CreateStorageBufferAndBindMem(const VkDevice device,
                                                    const VkDeviceSize size,
                                                    const std::vector<uint32_t> &queueFamilyIndices,
                                                    const VkPhysicalDeviceMemoryProperties *memProps,
                                                    VkBuffer *storageBuffer,
                                                    VkDeviceMemory *storageBufferMemory) {
    VkResult ret = CreateBuffer(device, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
                                queueFamilyIndices, storageBuffer);
    if (ret != VK_SUCCESS) {
        std::cout << "vkCreateBuffer failed, err=" << string_VkResult(ret) << std::endl;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, *storageBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.pNext = nullptr;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = GetRequiredMemTypeIndex(memProps, memRequirements,
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    ret = vkAllocateMemory(device, &allocInfo, nullptr, storageBufferMemory);
    if (ret != VK_SUCCESS) {
        std::cout << "vkAllocateMemory failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    ret = vkBindBufferMemory(device, *storageBuffer, *storageBufferMemory, 0);
    if (ret != VK_SUCCESS) {
        std::cout << "vkBindBufferMemory failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}
