//
// Created by neo on 25-7-1.
//

#ifndef ROPEOPERATOR_H
#define ROPEOPERATOR_H
#include "engine/compute/operators/UnaryOperator.h"

struct RoPEOperatorParams {
    uint32_t ropeTheta;
    uint32_t m;
};

class RoPEOperator final : public UnaryOperator {
    RoPEOperatorParams params{};

public:
    RoPEOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                 const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~RoPEOperator() override;

    void SetRopeTheta(const uint32_t ropeTheta) { this->params.ropeTheta = ropeTheta; }

    void SetM(const uint32_t m) { this->params.m = m; }

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void Destroy() override;
};


#endif //ROPEOPERATOR_H
