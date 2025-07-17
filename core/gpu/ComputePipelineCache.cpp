//
// Created by neo on 25-7-17.
//

#include "ComputePipelineCache.h"

#include "compute_graph/ComputePipelineNode.h"
#include <cassert>
#include <unistd.h>

static std::shared_ptr<ComputePipelineCache> instance = nullptr;

std::shared_ptr<ComputePipelineCache> ComputePipelineCache::GetInstance() {
    if (instance == nullptr) {
        instance = std::make_shared<ComputePipelineCache>();
    }
    return instance;
}

VkPipeline ComputePipelineCache::GetComputePipeline(const std::string& shaderPath) {
    if (this->pipelineCache.contains(shaderPath)) {
        return this->pipelineCache[shaderPath];
    }
    return VK_NULL_HANDLE;
}

VkPipeline ComputePipelineCache::CacheComputePipeline(const std::string& shaderPath, const VkPipeline pipeline) {
    assert(pipeline != VK_NULL_HANDLE);
    if (!this->pipelineCache.contains(shaderPath)) {
        this->pipelineCache.emplace(shaderPath, pipeline);
    }
    return pipeline;
}
