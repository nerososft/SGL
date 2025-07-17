//
// Created by neo on 25-7-17.
//

#ifndef COMPUTEPIPELINECACHE_H
#define COMPUTEPIPELINECACHE_H
#include <map>
#include <memory>
#include <string>
#include <vulkan/vulkan_core.h>

class ComputePipelineCache {
    std::map<std::string, VkPipeline> pipelineCache;
    std::map<std::string, VkShaderModule> shaderModuleCache;

public:
    ComputePipelineCache()  = default;
    ~ComputePipelineCache() = default;
    static std::shared_ptr<ComputePipelineCache> GetInstance();

    VkPipeline GetComputePipeline(const std::string& shaderPath);
    void CacheComputePipeline(const std::string& shaderPath, VkPipeline pipeline);

    VkShaderModule GetShaderModule(const std::string& shaderPath);
    void CacheShaderModule(const std::string& string, VkShaderModule vk_shader_module);
};

#endif // COMPUTEPIPELINECACHE_H
