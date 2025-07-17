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
        ++(*this->pipelineCache[shaderPath].refCount);
        return this->pipelineCache[shaderPath].pipeline;
    }
    return nullptr;
}

uint32_t ComputePipelineCache::PutComputePipeline(const std::string& shaderPath) {
    if (this->pipelineCache.contains(shaderPath)) {
        --(*this->pipelineCache[shaderPath].refCount);
        if (*(this->pipelineCache[shaderPath].refCount) == 0) {
            this->pipelineCache.erase(shaderPath);
            return 0;
        }
        return this->pipelineCache[shaderPath].refCount->load();
    }
    return 0;
}

VkPipeline ComputePipelineCache::CacheComputePipeline(const std::string& shaderPath, const VkPipeline pipeline) {
    assert(pipeline != VK_NULL_HANDLE);
    if (!this->pipelineCache.contains(shaderPath)) {
        Pipeline pp;
        pp.pipeline                     = pipeline;
        pp.refCount                     = std::make_unique<std::atomic<uint32_t>>(1);
        this->pipelineCache[shaderPath] = std::move(pp);
    }
    return pipeline;
}
