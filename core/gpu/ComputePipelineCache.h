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

public:
    ComputePipelineCache()  = default;
    ~ComputePipelineCache() = default;

    static std::shared_ptr<ComputePipelineCache> GetInstance();

    VkPipeline GetComputePipeline(const std::string& shaderPath);
    VkPipeline CacheComputePipeline(const std::string& shaderPath, VkPipeline pipeline);
};

#endif // COMPUTEPIPELINECACHE_H
