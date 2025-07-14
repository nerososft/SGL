//
// Created by neo on 25-6-28.
//

#ifndef OPERATOR_H
#define OPERATOR_H
#include "IOperator.h"
#include "core/gpu/VkGPUContext.h"


class Operator : public IOperator {
public:
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;

    explicit Operator(const std::shared_ptr<VkGPUContext> &gpuCtx);

    ~Operator() override;

    void Destroy() override;
};


#endif //OPERATOR_H
