//
// Created by neo on 25-6-28.
//

#ifndef SPLITOPERATOR_H
#define SPLITOPERATOR_H
#include "core/gpu/VkGPUBuffer.h"
#include "core/gpu/VkGPUContext.h"
#include "engine/ml/operators/Operator.h"

struct SplitOperatorParams {
    uint32_t dim;
    uint32_t nums;
};

class SplitOperator final : public Operator {
    SplitOperatorParams params{};
    std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
    std::vector<std::shared_ptr<VkGPUBuffer> > outputBuffers;

public:
    SplitOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                  const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                  const std::vector<std::shared_ptr<VkGPUBuffer> > &outputBuffers);

    ~SplitOperator() override;

    void SetDim(const size_t dim) { this->params.dim = dim; }

    void SetNums(const size_t nums) { this->params.nums = nums; }

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void Destroy() override;
};


#endif //SPLITOPERATOR_H
