//
// Created by neo on 25-6-28.
//

#ifndef CONCATOPERATOR_H
#define CONCATOPERATOR_H
#include <memory>

#include "core/gpu/VkGPUBuffer.h"
#include "core/gpu/VkGPUContext.h"
#include "core/gpu/compute_graph/IComputeGraphNode.h"
#include "engine/ml/operators/Operator.h"

struct ConcatOperatorParams {
    uint32_t dim;
    uint32_t nums;
    uint32_t dup;
};

class ConcatOperator final : public Operator {
    ConcatOperatorParams params{};
    std::vector<std::shared_ptr<VkGPUBuffer> > inputBuffers;
    std::shared_ptr<VkGPUBuffer> outBuffer = nullptr;

public:
    ConcatOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<std::shared_ptr<VkGPUBuffer> > &inputBuffers,
                   const std::shared_ptr<VkGPUBuffer> &outBuffer);

    ~ConcatOperator() override;

    void SetDim(const size_t dim) { this->params.dim = dim; }

    void SetNums(const size_t nums) { this->params.nums = nums; }

    void SetDup(const size_t dup) { this->params.dup = dup; }

    std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() override;

    void Destroy() override;
};

#endif //CONCATOPERATOR_H
