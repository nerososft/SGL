//
// Created by neo on 25-5-25.
//

#ifndef TANHOPERATOR_H
#define TANHOPERATOR_H

#include "core/gpu/VkGPUBuffer.h"
#include "core/gpu/VkGPUContext.h"
#include "engine/ml/operators/UnaryOperator.h"

class TanhOperator final : public UnaryOperator {
public:
    TanhOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                 const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                 const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~TanhOperator() override;

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void Destroy() override;
};

#endif //TANHOPERATOR_H
