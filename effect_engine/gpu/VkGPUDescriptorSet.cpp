//
// Created by neo on 2025/3/9.
//

#include "VkGPUDescriptorSet.h"

#include "VkGPUHelper.h"

VkGPUDescriptorSet::VkGPUDescriptorSet(const VkDevice device,
                                       const VkPipelineLayout pipelineLayout,
                                       const VkDescriptorSetLayout descriptorSetLayout) {
    this->device = device;
    this->pipelineLayout = pipelineLayout;
    this->descriptorSetLayout = descriptorSetLayout;
}

VkGPUDescriptorSet::~VkGPUDescriptorSet() {
    if (descriptorPool == VK_NULL_HANDLE) {
        return;
    }
    std::vector<VkDescriptorSet> descriptorSets;
    descriptorSets.push_back(descriptorSet);
    vkFreeDescriptorSets(device, descriptorPool, descriptorSets.size(), descriptorSets.data());
}

VkResult VkGPUDescriptorSet::AllocateDescriptorSets(const VkDescriptorPool descriptorPool) {
    this->descriptorPool = descriptorPool;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    descriptorSetLayouts.push_back(this->descriptorSetLayout);
    this->writeDescriptorSets.clear();
    return VkGPUHelper::AllocateDescriptorSets(device, descriptorPool, descriptorSetLayouts, &this->descriptorSet);
}

void VkGPUDescriptorSet::AddStorageBufferDescriptorSet(const uint32_t dtsBinding,
                                                       const VkDescriptorBufferInfo &descriptorBufferInfo) {
    this->writeDescriptorSets.push_back(
        VkGPUHelper::BuildWriteStorageBufferDescriptorSet(this->descriptorSet, dtsBinding, &descriptorBufferInfo)
    );
}

void VkGPUDescriptorSet::UpdateDescriptorSets() const {
    vkUpdateDescriptorSets(this->device,
                           this->writeDescriptorSets.size(),
                           this->writeDescriptorSets.data(),
                           0,
                           nullptr);
}

void VkGPUDescriptorSet::GPUCmdBindDescriptorSets(const VkCommandBuffer commandBuffer,
                                                  const VkPipelineBindPoint pipelineBindPoint) const {
    std::vector<VkDescriptorSet> descriptorSets;
    descriptorSets.push_back(this->descriptorSet);
    vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint,
                            this->pipelineLayout,
                            0,
                            descriptorSets.size(),
                            descriptorSets.data(),
                            0,
                            nullptr);
}

void VkGPUDescriptorSet::Destroy() const {
    vkFreeDescriptorSets(device, descriptorPool, 1, &descriptorSet);
}
