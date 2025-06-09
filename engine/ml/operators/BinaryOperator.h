//
// Created by neo on 2025/3/29.
//

#ifndef BINARYOPERATOR_H
#define BINARYOPERATOR_H
#include "IOperator.h"
#include "core/gpu/VkGPUBuffer.h"


class BinaryOperator : public IOperator {
public:
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<VkGPUBuffer> inputBuffer1 = nullptr;
    std::shared_ptr<VkGPUBuffer> inputBuffer2 = nullptr;
    std::shared_ptr<VkGPUBuffer> outputBuffer = nullptr;

    BinaryOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::shared_ptr<VkGPUBuffer> &inputBuffer1,
                   const std::shared_ptr<VkGPUBuffer> &inputBuffer2,
                   const std::shared_ptr<VkGPUBuffer> &outputBuffer);

    ~BinaryOperator() override;

    void Destroy() override;
};


#endif //BINARYOPERATOR_H
