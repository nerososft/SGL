//
// Created by 1234 on 2025/3/6.
//

#include "VkGPUHelper.h"

#include <iostream>
#include <ostream>
#include <vulkan/vk_enum_string_helper.h>

#include "effect_engine/utils/IOUtils.h"

VkResult VkGPUHelper::CreateShaderModule(const VkDevice device,
                                         const size_t shaderCodeSize,
                                         const uint32_t *shaderSpvCode,
                                         VkShaderModule *shaderModule) {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.flags = 0;
    createInfo.pNext = nullptr;
    createInfo.codeSize = shaderCodeSize;
    createInfo.pCode = shaderSpvCode;

    const VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, shaderModule);
    if (result != VK_SUCCESS) {
        std::cout << "failed to create shader module, err=" << string_VkResult(result) << std::endl;
    }
    return result;
}

VkResult VkGPUHelper::CreateShaderModuleFromPath(VkDevice device,
                                                 const std::string &shaderFilePath,
                                                 VkShaderModule *shaderModule) {
    std::vector<char> shaderSpvCode = IOUtils::ReadFile(shaderFilePath);
    if (shaderSpvCode.empty()) {
        return VK_ERROR_INVALID_SHADER_NV;
    }
    const VkResult result = CreateShaderModule(device,
                                               shaderSpvCode.size(),
                                               reinterpret_cast<uint32_t *>(shaderSpvCode.data()),
                                               shaderModule);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create shader module, err=" << string_VkResult(result) << std::endl;
    }
    return result;
}

VkResult VkGPUHelper::CreateComputePipeline(const VkDevice device,
                                            const VkPipelineCache pipelineCache,
                                            const VkPipelineLayout pipelineLayout,
                                            const VkShaderModule shaderModule,
                                            VkPipeline *pipeline) {
    VkPipelineShaderStageCreateInfo shaderStageInfo = {};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.flags = 0;
    shaderStageInfo.pNext = nullptr;
    shaderStageInfo.pName = "main";
    shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageInfo.module = shaderModule;
    shaderStageInfo.pSpecializationInfo = nullptr;

    std::vector<VkComputePipelineCreateInfo> pipelineCreateInfos;
    VkComputePipelineCreateInfo computePipelineCreateInfo;
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.flags = 0;
    computePipelineCreateInfo.pNext = nullptr;
    computePipelineCreateInfo.basePipelineIndex = 0;
    computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    computePipelineCreateInfo.layout = pipelineLayout;
    computePipelineCreateInfo.stage = shaderStageInfo;
    pipelineCreateInfos.push_back(computePipelineCreateInfo);
    const VkResult result = vkCreateComputePipelines(device,
                                                     pipelineCache,
                                                     pipelineCreateInfos.size(),
                                                     pipelineCreateInfos.data(),
                                                     nullptr,
                                                     pipeline);
    if (result != VK_SUCCESS) {
        std::cout << "failed to create compute pipeline, err=" << string_VkResult(result) << std::endl;
    }
    return result;
}

VkResult VkGPUHelper::CreatePipelineLayout(const VkDevice device,
                                           const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts,
                                           const std::vector<VkPushConstantRange> &pushConstantRanges,
                                           VkPipelineLayout *pipelineLayout) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.flags = 0;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = pushConstantRanges.size();
    pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();

    const VkResult ret = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, pipelineLayout);
    if (ret != VK_SUCCESS) {
        std::cout << "failed to create pipeline layout, " << string_VkResult(ret) << std::endl;
    }
    return ret;
}

VkResult VkGPUHelper::CreateDescriptorSetLayout(const VkDevice device,
                                                const std::vector<VkDescriptorSetLayoutBinding> &
                                                descriptorSetLayoutBindings,
                                                VkDescriptorSetLayout *descriptorSetLayout) {
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.flags = 0;
    descriptorSetLayoutCreateInfo.pNext = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount = descriptorSetLayoutBindings.size();
    descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

    const VkResult ret = vkCreateDescriptorSetLayout(device,
                                                     &descriptorSetLayoutCreateInfo,
                                                     nullptr,
                                                     descriptorSetLayout);
    if (ret != VK_SUCCESS) {
        std::cerr << "failed to create descriptor set layout, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}

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
