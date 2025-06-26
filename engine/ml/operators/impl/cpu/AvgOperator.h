//
// Created by neo on 25-6-26.
//

#ifndef AVGOPERATOR_H
#define AVGOPERATOR_H
#include "core/gpu/VkGPUBuffer.h"
#include "engine/ml/operators/IOperator.h"

class CPUAvgNode final : public IComputeGraphNode {
    std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
    float *avg = nullptr;

public:
    CPUAvgNode();

    ~CPUAvgNode() override = default;

    VkResult CreateComputeGraphNode() override;

    void Compute(VkCommandBuffer commandBuffer) override;

    void Destroy() override;

    void SetInputBuffer(const std::shared_ptr<VkGPUBuffer> &inputBuffer) { this->inputBuffer = inputBuffer; }

    void setAvg(float *avg) { this->avg = avg; }
};


class AvgOperator final : IOperator {
    std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
    float avg = 0.0;

public:
    explicit AvgOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer);

    ~AvgOperator() override = default;

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    float *GetAvg() { return &avg; }

    void Destroy() override;
};

#endif //AVGOPERATOR_H
