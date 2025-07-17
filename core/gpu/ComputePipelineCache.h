//
// Created by neo on 25-7-17.
//

#ifndef COMPUTEPIPELINECACHE_H
#define COMPUTEPIPELINECACHE_H
#include <map>
#include <memory>
#include <string>
#include <vulkan/vulkan_core.h>

typedef struct {
    VkPipeline pipeline;
    std::unique_ptr<std::atomic<uint32_t>> refCount;
} Pipeline;

class ComputePipelineCache {
    std::map<std::string, Pipeline> pipelineCache;

public:
    ComputePipelineCache()  = default;
    ~ComputePipelineCache() = default;

    static std::shared_ptr<ComputePipelineCache> GetInstance();

    VkPipeline GetComputePipeline(const std::string& shaderPath);
    uint32_t PutComputePipeline(const std::string& shaderPath);
    VkPipeline CacheComputePipeline(const std::string& shaderPath, VkPipeline pipeline);
};

#endif // COMPUTEPIPELINECACHE_H
