//
// Created by neo on 2025/3/25.
//

#ifndef VKGPUGRAPHICSPIPELINE_H
#define VKGPUGRAPHICSPIPELINE_H
#include <vector>
#include <vulkan/vulkan_core.h>


class VkGPUGraphicsPipeline {
    VkDevice device = VK_NULL_HANDLE;
    std::string vertexShaderPath;
    std::string fragShaderPath;

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    std::vector<VkPushConstantRange> pushConstantRanges;

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
    VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline = VK_NULL_HANDLE;

public:
    VkGPUGraphicsPipeline(const std::string &vertexShaderPath,
                          const std::string &fragShaderPath,
                          const std::vector<VkDescriptorSetLayoutBinding> &descriptorSetLayoutBindings,
                          const std::vector<VkPushConstantRange> &pushConstantRanges);

    ~VkGPUGraphicsPipeline() = default;

    VkResult CreateGraphicsPipeline(VkDevice device, VkPipelineCache pipelineCache, VkRenderPass renderPass);

    [[nodiscard]] VkPipelineLayout GetPipelineLayout() const { return this->pipelineLayout; }

    [[nodiscard]] VkDescriptorSetLayout GetDescriptorSetLayout() const { return this->descriptorSetLayout; }

    void GPUCmdBindPipeline(VkCommandBuffer commandBuffer) const;

    void Destroy() const;
};


#endif //VKGPUGRAPHICSPIPELINE_H
