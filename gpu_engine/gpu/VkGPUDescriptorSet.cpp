//
// Created by neo on 2025/3/9.
//

#include "VkGPUDescriptorSet.h"

#include "VkGPUHelper.h"
#include "log/Log.h"

VkGPUDescriptorSet::VkGPUDescriptorSet(const VkDevice device,
                                       const VkPipelineLayout pipelineLayout,
                                       const VkDescriptorSetLayout descriptorSetLayout) {
    this->device = device;
    this->pipelineLayout = pipelineLayout;
    this->descriptorSetLayout = descriptorSetLayout;
}

VkGPUDescriptorSet::~VkGPUDescriptorSet() {
    Destroy();
}

VkResult VkGPUDescriptorSet::AllocateDescriptorSets(const VkDescriptorPool descriptorPool) {
    this->descriptorPool = descriptorPool;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    descriptorSetLayouts.push_back(this->descriptorSetLayout);
    this->writeDescriptorSets.clear();
    const VkResult ret = VkGPUHelper::AllocateDescriptorSets(device, descriptorPool, descriptorSetLayouts, &this->descriptorSet);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to allocate descriptor sets!" << std::endl;
        return ret;
    }
    this->descriptorSets.push_back(this->descriptorSet);
    return ret;
}

void VkGPUDescriptorSet::AddStorageBufferDescriptorSet(const uint32_t dtsBinding,
                                                       const VkDescriptorBufferInfo &descriptorBufferInfo) {
    this->writeDescriptorSets.push_back(
        VkGPUHelper::BuildWriteStorageBufferDescriptorSet(this->descriptorSet, dtsBinding, &descriptorBufferInfo)
    );
}

void VkGPUDescriptorSet::UpdateDescriptorSets() const {
    if (descriptorSet == VK_NULL_HANDLE) {
        Logger() << "Update descriptor set to VK_NULL_HANDLE failed, descriptor not Allocated." << std::endl;
        return;
    }
    vkUpdateDescriptorSets(this->device,
                           this->writeDescriptorSets.size(),
                           this->writeDescriptorSets.data(),
                           0,
                           nullptr);
}

void VkGPUDescriptorSet::GPUCmdBindDescriptorSets(const VkCommandBuffer commandBuffer,
                                                  const VkPipelineBindPoint pipelineBindPoint) const {
    if (descriptorSet == VK_NULL_HANDLE) {
        Logger() << "Binding descriptor set to VK_NULL_HANDLE failed, descriptor not Allocated." << std::endl;
        return;
    }
    vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint,
                            this->pipelineLayout,
                            0,
                            descriptorSets.size(),
                            descriptorSets.data(),
                            0,
                            nullptr);
}

void VkGPUDescriptorSet::Destroy() {
    if (descriptorPool == VK_NULL_HANDLE) {
        for (VkDescriptorSet descriptorSet: this->descriptorSets) {
            if (descriptorSet != VK_NULL_HANDLE) {
                vkFreeDescriptorSets(this->device, descriptorPool, 1, &descriptorSet);
            }
        }
        this->descriptorSets.clear();
    }
}
