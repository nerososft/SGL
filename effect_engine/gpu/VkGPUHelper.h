//
// Created by 1234 on 2025/3/6.
//

#ifndef VKHELPER_H
#define VKHELPER_H
#include <vector>
#include <vulkan/vulkan_core.h>
#include <string>

class VkGPUHelper {
public:
    VkGPUHelper() = default;

    ~VkGPUHelper() = default;

    static VkResult CreateUniformBufferAndUploadData(VkDevice device,
                                                     const std::vector<uint32_t> &queueFamilyIndices,
                                                     const VkPhysicalDeviceMemoryProperties *memoryProperties,
                                                     VkDeviceSize bufferSize,
                                                     VkBuffer *buffer,
                                                     VkDeviceMemory *bufferMemory,
                                                     const void *uploadData);

    static VkResult CreateStorageBufferAndUploadData(VkDevice device,
                                                     const std::vector<uint32_t> &queueFamilyIndices,
                                                     const VkPhysicalDeviceMemoryProperties *memoryProperties,
                                                     VkDeviceSize bufferSize,
                                                     VkBuffer *buffer,
                                                     VkDeviceMemory *bufferMemory,
                                                     const void *uploadData);

    static VkSubmitInfo BuildSubmitInfo(const VkFlags *submitWaitDstStageMask,
                                        const std::vector<VkCommandBuffer> &submitCommandBuffers,
                                        const std::vector<VkSemaphore> &submitSignalSemaphores,
                                        const std::vector<VkSemaphore> &submitWaitSemaphores);

    static VkBufferMemoryBarrier BuildBufferMemoryBarrier(VkAccessFlagBits srcAccessMask,
                                                          VkAccessFlagBits dstAccessMask,
                                                          VkBuffer buffer,
                                                          VkDeviceSize size);

    static void GPUCmdPipelineBufferMemBarrier(VkCommandBuffer commandBuffer,
                                               VkPipelineStageFlags srcStageMask,
                                               VkPipelineStageFlags dstStageMask,
                                               VkDependencyFlags dependencyFlags,
                                               const std::vector<VkBufferMemoryBarrier> &bufferMemoryBarriers);

    static void GPUCmdPipelineBarrier(VkCommandBuffer commandBuffer,
                                      VkPipelineStageFlags srcStageMask,
                                      VkPipelineStageFlags dstStageMask,
                                      VkDependencyFlags dependencyFlags,
                                      const std::vector<VkMemoryBarrier> &memoryBarriers,
                                      const std::vector<VkBufferMemoryBarrier> &bufferMemoryBarriers,
                                      const std::vector<VkImageMemoryBarrier> &imageMemoryBarriers);

    static VkResult GPUQueueSubmit(VkQueue queue, const std::vector<VkSubmitInfo> &submitInfos, VkFence fence);

    static void GPUCmdDispatch(VkCommandBuffer commandBuffer,
                               uint32_t groupCountX,
                               uint32_t groupCountY,
                               uint32_t groupCountZ);

    static void GPUCmdPushConstant(VkCommandBuffer commandBuffer,
                                   VkPipelineLayout pipelineLayout,
                                   VkShaderStageFlagBits shaderStageFlag,
                                   uint32_t offset,
                                   size_t pushConstantSize,
                                   const void *pushConstantData);

    static void GPUEndCommandBuffer(VkCommandBuffer commandBuffer);

    static void GPUBeginCommandBuffer(VkCommandBuffer commandBuffer);

    static VkResult AllocateCommandBuffers(VkDevice device,
                                           VkCommandPool commandPool,
                                           uint32_t commandBufferCount,
                                           VkCommandBuffer *commandBuffers);

    static VkResult CreateFence(VkDevice device, VkFence *fence);

    static VkWriteDescriptorSet BuildWriteDescriptorSet(VkDescriptorSet descriptorSet,
                                                        uint32_t dtsBinding, VkDescriptorType type,
                                                        const VkDescriptorImageInfo *imageInfo,
                                                        const VkDescriptorBufferInfo *bufferInfo,
                                                        const VkBufferView *texelBufferView);

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

    static VkResult CreateStorageBufferAndBindMem(VkDevice device,
                                                  VkDeviceSize size,
                                                  const std::vector<uint32_t> &queueFamilyIndices,
                                                  const VkPhysicalDeviceMemoryProperties *memProps,
                                                  VkBuffer *storageBuffer,
                                                  VkDeviceMemory *storageBufferMemory);

    static VkResult CreateUniformBufferAndBindMem(VkDevice device,
                                                  VkDeviceSize size,
                                                  const std::vector<uint32_t> &queueFamilyIndices,
                                                  const VkPhysicalDeviceMemoryProperties *memProps,
                                                  VkBuffer *storageBuffer,
                                                  VkDeviceMemory *storageBufferMemory);
};


#endif //VKHELPER_H
