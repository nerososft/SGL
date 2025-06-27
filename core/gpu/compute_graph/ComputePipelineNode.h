//
// Created by neo on 2025/3/11.
//

#ifndef PIPELINECOMPUTEGRAPHNODE_H
#define PIPELINECOMPUTEGRAPHNODE_H

#include <functional>

#include "IComputeGraphNode.h"
#include "core/gpu/VkGPUComputePipeline.h"
#include "core/gpu/VkGPUContext.h"
#include "core/gpu/VkGPUDescriptorSet.h"


struct ComputeElement {
    PushConstantInfo pushConstantInfo;
    const std::vector<PipelineNodeBuffer> buffers;
    std::function<void(VkCommandBuffer commandBuffer)> customDrawFunc;
    std::function<void()> preCompute;
};

class ComputePipelineNode final : public IComputeGraphNode {
    std::string shaderPath;

    uint32_t workGroupCountX = 1;
    uint32_t workGroupCountY = 1;
    uint32_t workGroupCountZ = 1;

    uint32_t pushConstantSize = 0;
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<VkGPUComputePipeline> computePipeline = nullptr;
    std::vector<std::shared_ptr<VkGPUDescriptorSet> > pipelineDescriptorSets;

    std::vector<ComputeElement> computeElements;

public:
    ComputePipelineNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                        const std::string &name,
                        const std::string &shaderPath,
                        uint32_t pushConstantSize,
                        const std::vector<VkDescriptorSetLayoutBinding> &descriptorSetLayoutBindings,
                        uint32_t workGroupCountX,
                        uint32_t workGroupCountY,
                        uint32_t workGroupCountZ);

    [[nodiscard]] std::shared_ptr<VkGPUDescriptorSet> CreateDescriptorSet(const ComputeElement &computeElement) const;

    void AddComputeElement(const ComputeElement &computeElement);

    [[nodiscard]] std::vector<ComputeElement> GetComputeElements() const { return computeElements; }

    ~ComputePipelineNode() override = default;

    VkResult CreateComputeGraphNode() override;

    void Compute(VkCommandBuffer commandBuffer) override;

    void Destroy() override;
};


#endif //PIPELINECOMPUTEGRAPHNODE_H
