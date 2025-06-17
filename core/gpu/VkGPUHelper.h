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

    static VkDescriptorSetLayoutBinding BuildDescriptorSetLayoutBinding(uint32_t binding,
                                                                        VkDescriptorType descriptorType,
                                                                        uint32_t descriptorCount,
                                                                        VkShaderStageFlagBits shaderStageFlag);

    static VkWriteDescriptorSet BuildWriteSamplerDescriptorSet(VkDescriptorSet descriptorSet,
                                                               uint32_t dstBinding,
                                                               const VkDescriptorImageInfo *imageInfo);

    static VkResult CreateSampler(VkDevice device,
                                  VkSampler *sampler);

    static VkResult CreateImageView(VkDevice device,
                                    VkImage image,
                                    VkImageViewType viewType,
                                    VkFormat format,
                                    VkImageAspectFlags aspectFlags,
                                    VkImageView *imageView);

    static VkResult CreateImage(VkDevice device,
                                float width,
                                float height,
                                VkImageType imageType,
                                VkFormat format,
                                VkImageUsageFlags usage,
                                VkSharingMode sharingMode,
                                const std::vector<uint32_t> &queueFamilies,
                                VkImageLayout initialLayout,
                                VkImage *image);

    static VkResult CreateFramebuffer(VkDevice device,
                                      uint32_t width,
                                      uint32_t height,
                                      const std::vector<VkImageView> &attachments,
                                      VkRenderPass renderPass, VkFramebuffer
                                      *framebuffer);

    static VkResult CreateSemaphore(VkDevice device, VkSemaphore *semaphore);

    static PFN_vkCmdPipelineBarrier2KHR GetVkCmdPipelineBarrier2Fn(VkDevice device);

    static void GPUCmdPipelineMemBarrier(VkCommandBuffer commandBuffer,
                                         VkPipelineStageFlags srcStageMask,
                                         VkPipelineStageFlags dstStageMask,
                                         VkDependencyFlags dependencyFlags,
                                         const std::vector<VkMemoryBarrier> &memoryBarriers);

    static VkMemoryBarrier BuildMemoryBarrier(VkAccessFlagBits srcAccessMask, VkAccessFlagBits dstAccessMask);

    static void GPUCmdEndRenderPass(VkCommandBuffer commandBuffer);

    static void GPUCmdBeginRenderPass(VkCommandBuffer commandBuffer,
                                      VkRenderPass renderPass,
                                      VkFramebuffer framebuffer,
                                      VkRect2D renderArea,
                                      const std::vector<VkClearValue> &clearValues);

    static VkResult CreateRenderPass(VkDevice device,
                                     const std::vector<VkAttachmentDescription> &attachments,
                                     const std::vector<VkSubpassDependency> &dependencies,
                                     const std::vector<VkSubpassDescription> &subPasses,
                                     VkRenderPass *renderPass);

    static VkResult CreateGraphicsPipeline(VkDevice device,
                                           VkPipelineCache pipelineCache,
                                           VkPipelineLayout pipelineLayout,
                                           VkShaderModule vertexShaderModule,
                                           VkShaderModule fragmentShaderModule,
                                           VkRenderPass renderPass,
                                           std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions,
                                           std::vector<VkVertexInputAttributeDescription>
                                           vertexInputAttributeDescriptions,
                                           VkPrimitiveTopology inputAssemblyTopology,
                                           VkBool32 primitiveRestartEnable,
                                           std::vector<VkViewport> viewports,
                                           std::vector<VkRect2D> viewportScissors,
                                           VkPolygonMode polygonMode,
                                           float lineWidth,
                                           std::vector<VkPipelineColorBlendAttachmentState> colorBlendStateCreateInfos,
                                           std::vector<VkDynamicState> dynamicStates, VkPipeline *pipeline);

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

    static VkImageMemoryBarrier BuildImageMemoryBarrier(VkAccessFlagBits srcAccessMask,
                                                        VkAccessFlagBits dstAccessMask,
                                                        VkImage image,
                                                        VkImageLayout oldLayout,
                                                        VkImageLayout newLayout);

    static void GPUCmdPipelineBufferMemBarrier(VkCommandBuffer commandBuffer,
                                               VkPipelineStageFlags srcStageMask,
                                               VkPipelineStageFlags dstStageMask,
                                               VkDependencyFlags dependencyFlags,
                                               const std::vector<VkBufferMemoryBarrier> &bufferMemoryBarriers);

    static void GPUCmdPipelineImageMemBarrier(VkCommandBuffer commandBuffer,
                                              VkPipelineStageFlags srcStageMask,
                                              VkPipelineStageFlags dstStageMask,
                                              VkDependencyFlags dependencyFlags,
                                              const std::vector<VkImageMemoryBarrier> &imageMemoryBarriers);

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

    static VkWriteDescriptorSet BuildWriteUniformBufferDescriptorSet(VkDescriptorSet descriptorSet, uint32_t dstBinding,
                                                                     const VkDescriptorBufferInfo *
                                                                     descriptorBufferInfo);

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
                                 VkBufferUsageFlags usage,
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
                                                  uint32_t memoryPropertyFlagsBits,
                                                  VkBuffer *storageBuffer,
                                                  VkDeviceMemory *storageBufferMemory);

    static VkResult CreateImageAndBindMem(VkDevice device,
                                          float width,
                                          float height,
                                          VkImageType imageType,
                                          VkFormat format,
                                          VkImageUsageFlags usage,
                                          VkSharingMode sharingMode,
                                          VkImageLayout initialLayout,
                                          const VkPhysicalDeviceMemoryProperties *memProps,
                                          const std::vector<uint32_t> &queueFamilies,
                                          VkImage *image,
                                          VkDeviceMemory *imageMemory);

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

    static VkResult CreateVertexBufferAndBindMem(VkDevice device,
                                                 VkDeviceSize size,
                                                 const std::vector<uint32_t> &queueFamilyIndices,
                                                 const VkPhysicalDeviceMemoryProperties *memProps,
                                                 VkBuffer *storageBuffer,
                                                 VkDeviceMemory *storageBufferMemory);

    static VkResult CreateIndexBufferAndBindMem(VkDevice device,
                                                VkDeviceSize size,
                                                const std::vector<uint32_t> &queueFamilyIndices,
                                                const VkPhysicalDeviceMemoryProperties *memProps,
                                                VkBuffer *storageBuffer,
                                                VkDeviceMemory *storageBufferMemory);
};


#endif //VKHELPER_H
