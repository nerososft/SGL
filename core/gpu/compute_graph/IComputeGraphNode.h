//
// Created by neo on 2025/3/11.
//

#ifndef COMPUTEGRAPHNODE_H
#define COMPUTEGRAPHNODE_H
#include <vector>
#include <string>
#include <vulkan/vulkan_core.h>
#include <memory>

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
    PIPELINE_NODE_SAMPLER,
} PipelineNodeBufferType;

typedef struct {
    union {
        VkDescriptorBufferInfo bufferInfo;
        VkDescriptorImageInfo imageInfo;
    };
} PipelineDescriptorInfo;

typedef struct {
    PipelineNodeBufferType type;

    union {
        struct {
            VkDeviceSize bufferSize;
            VkBuffer buffer;
        } buf;

        struct {
            VkImage image;
            VkImageView imageView;
            VkBuffer imageBuffer;
            VkDeviceMemory imageBufferMemory;
            VkImageLayout imageLayout;
            VkSampler sampler;
        } sampler;
    };
} PipelineNodeBuffer;

typedef enum {
    COMPUTE_GRAPH_NODE_CPU,
    COMPUTE_GRAPH_NODE_COMPUTE,
    COMPUTE_GRAPH_NODE_GRAPHICS,
    COMPUTE_GRAPH_NODE_BUF_2_BUF_COPY,
    COMPUTE_GRAPH_NODE_IMAGE_2_BUF_COPY,
    COMPUTE_GRAPH_NODE_BUF_2_IMAGE_COPY,
    COMPUTE_GRAPH_NODE_UNKNOWN,
} ComputeGraphNodeType;

class IComputeGraphNode {
protected:
    ComputeGraphNodeType type = COMPUTE_GRAPH_NODE_UNKNOWN;
    std::string name;
    std::vector<std::shared_ptr<IComputeGraphNode> > dependencies;

public:
    IComputeGraphNode() = default;

    virtual ~IComputeGraphNode() = default;

    virtual VkResult CreateComputeGraphNode() = 0;

    void AddDependenceNode(const std::shared_ptr<IComputeGraphNode> &node) { this->dependencies.push_back(node); }

    virtual void Compute(VkCommandBuffer commandBuffer) = 0;

    std::string &GetName() { return name; }

    [[nodiscard]] bool IsGraphics() const { return type == COMPUTE_GRAPH_NODE_GRAPHICS; }

    virtual void Destroy() {
        this->dependencies.clear();
        this->dependencies.resize(0);
    };
};


#endif //COMPUTEGRAPHNODE_H
