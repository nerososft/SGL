//
// Created by neo on 2025/3/9.
//

#ifndef VKGPUDESCRIPTORSET_H
#define VKGPUDESCRIPTORSET_H
#include <vector>
#include <vulkan/vulkan_core.h>


class VkGPUDescriptorSet {
    VkDevice device = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    std::vector<VkWriteDescriptorSet> writeDescriptorSets;

public:
    VkGPUDescriptorSet(VkDevice device,
                       VkPipelineLayout pipelineLayout,
                       VkDescriptorSetLayout descriptorSetLayout);

    VkResult AllocateDescriptorSets(VkDescriptorPool descriptorPool);

    void AddStorageBufferDescriptorSet(uint32_t dtsBinding, const VkDescriptorBufferInfo &descriptorBufferInfo);

    void UpdateDescriptorSets() const;

    void GPUCmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint) const;
};


#endif //VKGPUDESCRIPTORSET_H
