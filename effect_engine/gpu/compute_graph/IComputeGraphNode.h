//
// Created by neo on 2025/3/11.
//

#ifndef COMPUTEGRAPHNODE_H
#define COMPUTEGRAPHNODE_H
#include <vector>
#include <vulkan/vulkan_core.h>
#include<string>
#include <memory>
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
