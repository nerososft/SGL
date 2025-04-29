//
// Created by neo on 2025/3/29.
//

#ifndef UNARYOPERATOR_H
#define UNARYOPERATOR_H
#include "IOperator.h"
#include "gpu/VkGPUBuffer.h"

class UnaryOperator : public IOperator {
    std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> outputBuffer = nullptr;

public:
    UnaryOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer, const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~UnaryOperator() override;

    void Destroy() const;
};

#endif //UNARYOPERATOR_H
