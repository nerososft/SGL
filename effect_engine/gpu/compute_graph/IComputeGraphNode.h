//
// Created by neo on 2025/3/11.
//

#ifndef COMPUTEGRAPHNODE_H
#define COMPUTEGRAPHNODE_H
#include <vector>
#include <string>
#include <vulkan/vulkan_core.h>

typedef struct {
    VkDeviceSize size;
    void *data;
} PushConstantInfo;

typedef enum {
    PIPELINE_NODE_BUFFER_UNIFORM,
    PIPELINE_NODE_BUFFER_VERTEX,
    PIPELINE_NODE_BUFFER_INDEX,
    PIPELINE_NODE_BUFFER_STORAGE_READ,
    PIPELINE_NODE_BUFFER_STORAGE_WRITE,
} PipelineNodeBufferType;

typedef struct {
    PipelineNodeBufferType type;
    VkDeviceSize bufferSize;
    VkBuffer buffer;
} PipelineNodeBuffer;

class IComputeGraphNode {
protected:
    std::string name;
    std::vector<std::shared_ptr<IComputeGraphNode> > dependencies;

public:
    IComputeGraphNode() = default;

    virtual ~IComputeGraphNode() = default;

    virtual VkResult CreateComputeGraphNode() = 0;

    void AddDependenceNode(const std::shared_ptr<IComputeGraphNode> &node) { this->dependencies.push_back(node); }

    virtual void Compute(VkCommandBuffer commandBuffer) = 0;

    std::string &GetName() { return name; }

    virtual void Destroy() = 0;
};


#endif //COMPUTEGRAPHNODE_H
