//
// Created by neo on 25-6-27.
//

#ifndef SUMOPERATOR_H
#define SUMOPERATOR_H
#include "CPUComputeNode.h"
#include "core/gpu/VkGPUBuffer.h"
#include "engine/ml/operators/IOperator.h"

class CPUSumNode final : public CPUComputeNode {
    std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
    float *sum = nullptr;

public:
    CPUSumNode();

    ~CPUSumNode() override = default;

    VkResult CreateComputeGraphNode() override;

    void Compute(VkCommandBuffer commandBuffer) override;

    void Destroy() override;

    void SetInputBuffer(const std::shared_ptr<VkGPUBuffer> &inputBuffer) { this->inputBuffer = inputBuffer; }

    void SetSum(float *sum) { this->sum = sum; }
};


class SumOperator final : IOperator {
    std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
    float sum = 0.0;

public:
    explicit SumOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer);

    ~SumOperator() override = default;

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    float *GetSum() { return &sum; }

    void Destroy() override;
};


#endif //SUMOPERATOR_H
