//
// Created by neo on 2025/3/9.
//

#ifndef VKGPUCOMPUTEPIPELINE_H
#define VKGPUCOMPUTEPIPELINE_H
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>


class VkGPUComputePipeline {
    VkDevice device = VK_NULL_HANDLE;
    std::string computeShaderPath;
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    std::vector<VkPushConstantRange> pushConstantRanges;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkShaderModule computeShaderModule = VK_NULL_HANDLE;
    VkPipeline computePipeline = VK_NULL_HANDLE;

public:
    VkGPUComputePipeline(const std::string &computeShaderPath,
                         const std::vector<VkDescriptorSetLayoutBinding> &descriptorSetLayoutBindings,
                         const std::vector<VkPushConstantRange> &pushConstantRanges);

    ~VkGPUComputePipeline() = default;

    VkResult CreateComputePipeline(VkDevice device,
                                   VkPipelineCache pipelineCache);

    [[nodiscard]] VkPipelineLayout GetPipelineLayout() const { return this->pipelineLayout; }

    [[nodiscard]] VkDescriptorSetLayout GetDescriptorSetLayout() const { return this->descriptorSetLayout; }

    void GPUCmdBindPipeline(VkCommandBuffer commandBuffer) const;

    void Destroy() const;
};


#endif //VKGPUCOMPUTEPIPELINE_H
