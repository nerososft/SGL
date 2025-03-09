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

    static VkWriteDescriptorSet BuildWriteDescriptorSet(VkDescriptorSet descriptorSet,
                                                        uint32_t dtsBinding, VkDescriptorType type,
                                                        const VkDescriptorImageInfo *imageInfo,
                                                        const VkDescriptorBufferInfo *
                                                        bufferInfo, const VkBufferView *texelBufferView);

    static VkWriteDescriptorSet BuildWriteStorageBufferDescriptorSet(VkDescriptorSet descriptorSet,
                                                                     uint32_t dstBinding,
                                                                     const VkDescriptorBufferInfo *
                                                                     descriptorBufferInfo);

    static VkResult AllocateDescriptorSets(VkDevice device,
                                           VkDescriptorPool descriptorPool,
                                           const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts,
                                           VkDescriptorSet *descriptorSets);

    static VkResult CreateShaderModule(VkDevice device,
                                       size_t shaderCodeSize,
                                       const uint32_t *shaderSpvCode,
                                       VkShaderModule *shaderModule);

    static VkResult CreateShaderModuleFromPath(VkDevice device,
                                               const std::string &shaderFilePath,
                                               VkShaderModule *shaderModule);

    static VkResult CreateComputePipeline(VkDevice device,
                                          VkPipelineCache pipelineCache,
                                          VkPipelineLayout pipelineLayout,
                                          VkShaderModule shaderModule,
                                          VkPipeline *pipeline);

    static VkResult CreatePipelineLayout(VkDevice device,
                                         const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts,
                                         const std::vector<VkPushConstantRange> &pushConstantRanges,
                                         VkPipelineLayout *pipelineLayout);

    static VkResult CreateDescriptorSetLayout(VkDevice device,
                                              const std::vector<VkDescriptorSetLayoutBinding> &
                                              descriptorSetLayoutBindings,
                                              VkDescriptorSetLayout *descriptorSetLayout);

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
