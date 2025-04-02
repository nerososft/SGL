//
// Created by neo on 2025/3/29.
//

#ifndef BINARYOPERATOR_H
#define BINARYOPERATOR_H
#include "IOperator.h"
#include "effect_engine/gpu/VkGPUBuffer.h"


class BinaryOperator : public IOperator {
    std::shared_ptr<VkGPUBuffer> inputBuffer1 = nullptr;
    std::shared_ptr<VkGPUBuffer> inputBuffer2 = nullptr;
    std::shared_ptr<VkGPUBuffer> outputBuffer = nullptr;

public:
    BinaryOperator(const std::shared_ptr<VkGPUBuffer> &inputBuffer1,
                   const std::shared_ptr<VkGPUBuffer> &inputBuffer2,
                   const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~BinaryOperator() override;

    void Destroy() const;
};


#endif //BINARYOPERATOR_H
